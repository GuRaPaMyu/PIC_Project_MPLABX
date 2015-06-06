void adc_init(void);
int adc_read(unsigned char channel);
void adc_itrpt_init(void);
void adc_itrpt_go(unsigned char channel);
int adc_itrpt_read(void);