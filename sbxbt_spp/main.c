// SPP Firmware Copyright(C) Kunihiro Shibuya
#include <stdlib.h>
#define USE_AND_OR /* To enable AND_OR mask setting */
#include<ports.h>
#include<pps.h>
#include<timer.h>
#include<outcompare.h>
//#include "global.h"
#include "btstack/debug.h"
#include "btstack/btstack.h"
#include <btstack/hci_cmds.h>
#include <btstack/run_loop.h>
#include <btstack/sdp_util.h>

#include "btstack/hci.h"
#include "btstack/l2cap.h"
#include "btstack/btstack_memory.h"
#include "btstack/remote_device_db.h"
#include "btstack/rfcomm.h"
#include "btstack/sdp.h"
//#include "btstack/config.h"

#define	CREDITS					 10


#define	led1_on_mac()			{led1_on(); led1_on_count=timer_counter; led1_on_state=1;}
static uint8_t		rfcomm_send_credit=CREDITS;
static uint8_t		rfcomm_channel_nr = 1;
static uint16_t		rfcomm_channel_id;
static uint8_t		spp_service_buffer[128];
static unsigned		timer_counter=0;
static unsigned		led1_on_count=0;
static uint8_t		led1_on_state=0;
char				lineBuffer[32];
static uint8_t		startup_state=0;
#if USE_RSSI
static unsigned		rssi_blink_rate_ms;
#endif

#if SPP_MASTER
static bd_addr_t target_addr = TARGET_ADDR;
int rfcomm_channel = 1;
void *main_connection=0;
#endif

hci_transport_t * hci_transport_picusb_instance();

#if defined(__PIC24FJ64GB004__)||defined(__PIC24FJ64GB002__)
	_CONFIG1(WDTPS_PS2048 & FWPSA_PR128 & WINDIS_OFF & FWDTENDEF & ICSDEF & GWRP_OFF & GCP_OFF & JTAGEN_OFF)
	_CONFIG2(POSCMOD_NONE & I2C1SEL_PRI & IOL1WAY_OFF & OSCIOFNC_ON & FCKSM_CSDCMD & FNOSC_FRCPLL & PLL96MHZ_ON & PLLDIV_DIV2 & IESO_OFF)
	_CONFIG3(WPFP_WPFP0 & SOSCSEL_IO & WUTSEL_LEG & WPDIS_WPDIS & WPCFG_WPCFGDIS & WPEND_WPENDMEM)
	_CONFIG4(DSWDTPS_DSWDTPS3 & DSWDTOSC_LPRC & RTCOSC_SOSC & DSBOREN_OFF & DSWDTEN_OFF)
#else

    #error Cannot define configuration bits.

#endif

BYTE        deviceAddress;  // Address of the device on the USB
bd_addr_t addr_global;

BOOL InitializeSystem ( void )
{
   #if defined(__PIC24FJ64GB004__)||defined(__PIC24FJ64GB002__)
	    {
	        unsigned int pll_startup_counter = 600;
	        CLKDIVbits.PLLEN = 1;
	        while(pll_startup_counter--);
	    }

		AD1PCFG = 0xffff;
	    CLKDIV = 0;    // Set PLL prescaler (1:1)
    #endif

	on_setup();
	led1_setup();
	led2_setup();
	u1rx_setup();
	u1tx_setup();
	u1cts_setup();
	u1rts_setup();
	sto_setup();
	assoc_setup();
	pairen_setup();

    // Init UART
    UART1Init(BAUDRATE1);
#if	STO_SERIAL
    UART2Init();
#endif

    return TRUE;
} // InitializeSystem

BOOL USB_ApplicationEventHandler ( BYTE address, USB_EVENT event, void *data, DWORD size )
{
    #ifdef USB_BLUETOOTH_SUPPORT_SERIAL_NUMBERS
        BYTE i;
    #endif

    // Handle specific events.
    switch (event)
    {
        case EVENT_BLUETOOTH_ATTACH:
            if (size == sizeof(BLUETOOTH_DEVICE_ID))
            {
                deviceAddress   = ((BLUETOOTH_DEVICE_ID *)data)->deviceAddress;
                log_info( "Bluetooth device attached - event, deviceAddress=%d\r\n",deviceAddress );
                #ifdef USB_BLUETOOTH_SUPPORT_SERIAL_NUMBERS
                    for (i=1; i<((BLUETOOTH_DEVICE_ID *)data)->serialNumberLength; i++)
                    {
                        log_info("%c", ((BLUETOOTH_DEVICE_ID *)data)->serialNumber[i] );
                    }
	                log_info( "\r\n" );
                #endif
                return TRUE;
            }
            break;

        case EVENT_BLUETOOTH_DETACH:
            deviceAddress   = 0;
            log_info( "Bluetooth device detached - event\r\n" );
            return TRUE;

        case EVENT_BLUETOOTH_TX_CMD_DONE:           // The main state machine will poll the driver.
            return TRUE;

        case EVENT_BLUETOOTH_TX_ACL_DONE:           // The main state machine will poll the driver.
            return TRUE;

        case EVENT_BLUETOOTH_RX_EVENT_DONE:
			if(*(DWORD*)data){
				event_bluetooth_rxEvent_done((WORD)(*(DWORD*)data));
			}
            return TRUE;

        case EVENT_BLUETOOTH_RX_ACL_DONE:
			if(*(DWORD*)data){
				event_bluetooth_rxAcl_done((WORD)(*(DWORD*)data));
			}
            return TRUE;

        case EVENT_VBUS_REQUEST_POWER:
            // We'll let anything attach.
            return TRUE;

        case EVENT_VBUS_RELEASE_POWER:
            // We aren't keeping track of power.
            return TRUE;

        case EVENT_HUB_ATTACH:
            log_info( "\r\n***** USB Error - hubs are not supported *****\r\n" );
            return TRUE;
            break;

        case EVENT_UNSUPPORTED_DEVICE:
            log_info( "\r\n***** USB Error - device is not supported *****\r\n" );
            return TRUE;
            break;

        case EVENT_CANNOT_ENUMERATE:
            log_info( "\r\n***** USB Error - cannot enumerate device *****\r\n" );
            return TRUE;
            break;

        case EVENT_CLIENT_INIT_ERROR:
            log_info( "\r\n***** USB Error - client driver initialization error *****\r\n" );
            return TRUE;
            break;

        case EVENT_OUT_OF_MEMORY:
            log_info( "\r\n***** USB Error - out of heap memory *****\r\n" );
            return TRUE;
            break;

        case EVENT_UNSPECIFIED_ERROR:   // This should never be generated.
            log_info( "\r\n***** USB Error - unspecified *****\r\n" );
            return TRUE;
            break;

        case EVENT_SUSPEND:
        case EVENT_DETACH:
        case EVENT_RESUME:
        case EVENT_BUS_ERROR:
            return TRUE;
            break;

        default:
            break;
    }

    return FALSE;

} // USB_ApplicationEventHandler

// Bluetooth logic
static void packet_handler (void * connection, uint8_t packet_type, uint16_t channel, uint8_t *packet, uint16_t size)
{
//int i,j;
	bd_addr_t	event_addr;
    uint8_t		rfcomm_channel_nr;
    uint8_t		rfcomm_channel_id_temp;
    uint16_t	mtu;
    
	switch (packet_type) {
		case HCI_EVENT_PACKET:
			switch (packet[0]) {
				case BTSTACK_EVENT_STATE:
					// bt stack activated, get started - set local name
					if (packet[2] == HCI_STATE_WORKING) {
						if(
							!addr_global[0]
						&&	!addr_global[1]
						&&	!addr_global[2]
						&&	!addr_global[3]
						&&	!addr_global[4]
						&&	!addr_global[5]
						){
							//bd address failed -> reset
							Reset();
						}
//hci_send_cmd(&hci_read_local_supprted_commands);
						xsprintf(lineBuffer,LOCAL_NAME "-%02x%02x%02x%02x%02x%02x",
							addr_global[0],
							addr_global[1],
							addr_global[2],
							addr_global[3],
							addr_global[4],
							addr_global[5]
						);
						log_info("local name:%s\r\n",lineBuffer);
#if	!__DEBUG
						DelayMs(100);
#endif
                        hci_send_cmd(&hci_write_local_name, lineBuffer);
					}
					break;

				case HCI_EVENT_COMMAND_COMPLETE:
#if	SPP_MASTER
					if (COMMAND_COMPLETE_EVENT(packet, hci_read_bd_addr)){
                        bt_flip_addr(event_addr, &packet[6]);
                        log_info("BD-ADDR: %s\n\r", bd_addr_to_str(event_addr));
                        break;
                    }
					if (COMMAND_COMPLETE_EVENT(packet, hci_write_local_name)){
						log_info("rfcomm_create_channel\n\r");
						startup_state=1;
						main_connection=connection;
						//rfcomm_create_channel_internal(connection,&target_addr,rfcomm_channel);
                        break;
                    }
#else	//SPP SLAVE
					if (COMMAND_COMPLETE_EVENT(packet, hci_read_bd_addr)){
                        bt_flip_addr(event_addr, &packet[6]);
                        log_info("BD-ADDR: %s\n\r", bd_addr_to_str(event_addr));
                        break;
                    }
					if (COMMAND_COMPLETE_EVENT(packet, hci_write_local_name)){
    				    hci_send_cmd(&hci_write_class_of_device, 0);
                        break;
                    }
					if (COMMAND_COMPLETE_EVENT(packet, hci_write_class_of_device)){
						if(get_pairen()){
//hci_send_cmd(&hci_delete_stored_link_key,addr_global,1);
							hci_discoverable_control(1);
						}
						startup_state=1;
                        break;
					}
#endif
#if	USE_RSSI
					if(COMMAND_COMPLETE_EVENT(packet,hci_read_rssi)){
						signed char rssi_value;
						unsigned blink_rate;
						if(packet[5]==0){		//status=0
							rssi_value=(signed char)(packet[8]);
							log_info("hci_read_rssi status=%d RSSI=%d\n",packet[5],rssi_value);

							//blink rate
							if(rssi_value>=0){
								blink_rate=30;
							}else
							if(rssi_value<=-30){
								blink_rate=0;
							}else{
								blink_rate=(unsigned)rssi_value+30;
							}
							rssi_blink_rate_ms=blink_rate*60+200;
						}else{
							rssi_blink_rate_ms=1000;
						}
					}
#endif
                    break;

				case HCI_EVENT_LINK_KEY_REQUEST:
					// deny link key request
                    log_info("Link key request\n\r");
                    bt_flip_addr(event_addr, &packet[2]);
					hci_send_cmd(&hci_link_key_request_negative_reply, &event_addr);
					break;
					
				case HCI_EVENT_PIN_CODE_REQUEST:
					// inform about pin code request
                    bt_flip_addr(event_addr, &packet[2]);
                    log_info("Pin code request - using '" PIN_CODE_DEFAULT "'\n\r");
					hci_send_cmd(&hci_pin_code_request_reply, &event_addr, 4, PIN_CODE_DEFAULT);
					break;
                
                case RFCOMM_EVENT_INCOMING_CONNECTION:
					// data: event (8), len(8), address(48), channel (8), rfcomm_cid (16)
					bt_flip_addr(event_addr, &packet[2]); 
					rfcomm_channel_nr = packet[8];
					rfcomm_channel_id_temp = READ_BT_16(packet, 9);
					log_info("RFCOMM channel %u requested for %s\n\r", rfcomm_channel_nr, bd_addr_to_str(event_addr));
                    rfcomm_accept_connection_internal(rfcomm_channel_id_temp);
					break;
					
				case RFCOMM_EVENT_OPEN_CHANNEL_COMPLETE:
					// data: event(8), len(8), status (8), address (48), server channel(8), rfcomm_cid(16), max frame size(16)
					if (packet[2]) {
						log_info("RFCOMM channel open failed, status %u\n\r", packet[2]);
					} else {
						rfcomm_channel_id = READ_BT_16(packet, 12);
						mtu = READ_BT_16(packet, 14);
						log_info("\n\rRFCOMM channel open succeeded. New RFCOMM Channel ID %u, max frame size %u\n\r", rfcomm_channel_id, mtu);
					}
					break;
                    
                case RFCOMM_EVENT_CHANNEL_CLOSED:
                    rfcomm_channel_id = 0;
                    break;
                
                default:
                    break;
			}
            break;
        case RFCOMM_DATA_PACKET:
            // hack: truncate data (we know that the packet is at least on byte bigger
            //packet[size] = 0;
		    led1_on_mac();
			for(;size--;){
				UART1PutChar(*packet++);
			}
			if(!--rfcomm_send_credit){
	            rfcomm_send_credit = CREDITS;
		        rfcomm_grant_credits(rfcomm_channel_id, rfcomm_send_credit);
			}
			break;

        default:
            break;
	}
}

#if USE_RSSI
#define HEARTBEAT_PERIOD_MS		1000

static void  heartbeat_handler(struct timer *ts)
{
int rfcomm_read_rssi(uint16_t rfcomm_cid);
    if (rfcomm_channel_id){
        int err = rfcomm_read_rssi(rfcomm_channel_id);
        if (err) {
            xprintf("rfcomm_read_rssi -> error %d\n", err);
        }
    }
#if	SPP_MASTER
	else{
		//connection
		if(startup_state){
			rfcomm_create_channel_internal(main_connection,&target_addr,rfcomm_channel);
		}
	}
#endif

    run_loop_set_timer(ts, HEARTBEAT_PERIOD_MS);
    run_loop_add_timer(ts);
} 
#endif

int main ( void )
{
	RestartWatchdog();
#if	STO_SERIAL
	xdev_out(UART2PutChar);
#endif

    // Initialize the processor and peripherals.
    if ( InitializeSystem() != TRUE )
    {
        log_info( "\r\n\r\nCould not initialize " LOCAL_NAME " - system.  Halting.\r\n\r\n" );
        while (1);
    }
    if ( USBHostInit(0) == TRUE )
    {
        log_info( "\r\n\r\n***** " LOCAL_NAME " Initialized *****\r\n\r\n" );
    }
    else
    {
        log_info( "\r\n\r\nCould not initialize " LOCAL_NAME " - USB.  Halting.\r\n\r\n" );
        while (1);
    }

    // Main Processing Loop
    while(!deviceAddress)
    {
        // This demo does not check for overcurrent conditions.  See the
        // USB Host - Data Logger for an example of overcurrent detection
        // with the PIC24F and the USB PICtail Plus.

        // Maintain USB Host State
        USBHostTasks();

        //if (CheckForNewAttach()){
		//	break;
        //}
    }

	DelayMs(1000);

	led2_off();

	/// GET STARTED with BTstack ///
	btstack_memory_init();
    run_loop_init(RUN_LOOP_EMBEDDED);
	
    // init HCI
	hci_transport_t    * transport = hci_transport_picusb_instance();
	bt_control_t       * control   = NULL;
    hci_uart_config_t  * config    = NULL;
    remote_device_db_t * remote_db = (remote_device_db_t *) &remote_device_db_memory;
	hci_init(transport, config, control, remote_db);

    // use eHCILL
//    bt_control_cc256x_enable_ehcill(1);
    
    // init L2CAP
    l2cap_init();
    l2cap_register_packet_handler(packet_handler);
    
	// init RFCOMM
    rfcomm_init();
    rfcomm_register_packet_handler(packet_handler);
    rfcomm_register_service_with_initial_credits_internal(NULL, rfcomm_channel_nr, 100, CREDITS);  // reserved channel, mtu=100, 1 credit

    // init SDP, create record for SPP and register with SDP
    sdp_init();

	memset(spp_service_buffer, 0, sizeof(spp_service_buffer));
    service_record_item_t * service_record_item = (service_record_item_t *) spp_service_buffer;
    sdp_create_spp_service( (uint8_t*) &service_record_item->service_record, 1, "SPP");
    log_info("SDP service buffer size: %u\n\r", (uint16_t) (sizeof(service_record_item_t) + de_get_len((uint8_t*) &service_record_item->service_record)));
    sdp_register_service_internal(NULL, service_record_item);

#if USE_RSSI
    // set one-shot timer
    timer_source_t heartbeat;
    heartbeat.process = &heartbeat_handler;
    run_loop_set_timer(&heartbeat, HEARTBEAT_PERIOD_MS);
    run_loop_add_timer(&heartbeat);
#endif

 	// turn on!
	hci_power_control(HCI_POWER_ON);

#if	!USE_WRITE_STORED_LINK_KEY
extern linked_list_t db_mem_link_keys;
void load_link_keys(remote_device_db_t * lpremote_device_db);
void print_link_keys(linked_list_t list);
	load_link_keys(remote_db);
	print_link_keys(db_mem_link_keys);
#endif

    led1_off();

    // go!
    run_loop_execute();	

    return 0;

} // main


void hal_tick_init(void)
{
	usb_tick_handler=NULL;
}

void hal_tick_set_handler(void (*tick_handler)(void))
{
	usb_tick_handler=tick_handler;
}

void hal_tick_event(void)
{
	timer_counter++;

	RestartWatchdog();

#if	!__DEBUG
	if(!startup_state && timer_counter>5000){
		Reset();
	}
#endif

    if (rfcomm_channel_id){
        if(timer_counter&0x100){
            //led2_off();
			assoc_l();
        }else{
            //led2_on();
			assoc_h();
        }
#if	USE_RSSI
		if(rssi_blink_rate_ms && (timer_counter%rssi_blink_rate_ms)<100){
            led2_on();
        }else{
            led2_off();
        }
#else
        if(timer_counter&0x100){
            led2_off();
        }else{
            led2_on();
        }
#endif
		sto_h();
    }else{
        led2_on();
		sto_l();
		assoc_l();
    }

	if(led1_on_state && (timer_counter-led1_on_count)>LED1_ON_COUNT){
		led1_off();
		led1_on_state=0;
	}

	// call tick handler
	if(usb_tick_handler){
		usb_tick_handler();
	}
}

void sendchar(void)
{
	int	ch;
	unsigned len;

	if(is_rfcomm_send_avail(rfcomm_channel_id)){
		//can't send rfcomm now
		return;
	}

	for(len=0;len<sizeof(lineBuffer);len++){
		ch=UART1GetChar();
		if(ch<0){
			break;
		}
		//log_info("UART1GetChar=%c\r\n",ch);
		lineBuffer[len]=ch;
	}
	if(len){
	    led1_on_mac();
        int err = rfcomm_send_internal(rfcomm_channel_id, (uint8_t*) lineBuffer, len);
        if (err) {
            log_error("rfcomm_send_internal -> error %d", err);
        }
	}
}

/*************************************************************************
 * EOF main.c
 */

