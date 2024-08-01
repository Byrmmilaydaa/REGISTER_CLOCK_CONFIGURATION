#include "stm32f4xx.h"
#include "stm32f4_discovery.h"

//Reference manual diye dökümanýmýz var ve bu döküman bize stm32f4xx kartýný nasýl kodlayacaðýmýzý söylüyor.


extern uint32_t SystemCoreClock;
//bu kodun anlamý system_stm32f4xx dosyasý içerindesindeki tanýmlamýþ olduðum systemcoreclock ifadesini dosyama çaðýrýyorum.(168MHz olarak tanýmladým)
//extern ediyorsak ilk deðerini vermeyiz

uint32_t systemClock; //benim kendimin takip edeceði bir deðiþken tanýmladým.

//register=kaydedici demek yani biz bir mikrodenetleyiciye belli iþlemler yaptýrmak istiyorsak
//öncelikle o mikrodenetleyicinin o iþlemi yapabilecek noktalarýna belirli ayarlar yapmamýz lazým


//tüm çevresel birimlerde kontrol register adý altýnda bir register vardýr. Kontrol etmek için kullanýlýr.
//RCC=RESET AND CLOCK CONTROL anlamýna gelmektedir.
//bu bi register(kaydedici) bunun içinde belli bitler vardýr.Stm32f4xx kartý 32 bitliktir yani kaydedicilerinde 32 tane bitler var demektir.

void RCC_Config(void){ //yapýlmasý gereken ilk adýmdýr.Çünkü;

	RCC -> CR &= ~ (1 << 0);//ayrýca 0x00000083 þeklinde de yazabiliriz ayný adresi ifade eder.
	//HSI OFF, HSI=dahili osilatörler. Dahili osilatörler harici osilatörlere göre daha kalitesizdir. özellikle kritik uygulamalarda önemli bu durum
	//kontrol registerým resetlendi demek bu kod ile
	//&= bu iþaret kaydet demek
	//~ bu iþaret tersle demek.Bu þekilde (&= ~) kullanýlýrsa ise sadece 0.biti tersle demek
	RCC -> CR |= 1 << 16; //HSE ON HSE=harici osilatörler. //16.bite 1 yazmam gerekiyor ve bu þekilde sadece 16.bite yazmýþ oluyoruz.
	//|= bu iþaret daha önceki kullandýðýmýz &= bu iþarete +1 ekler. ya da eþittir yapar
	while(!(RCC -> CR & (1 << 17))); // Wait HSE active
	//HSE çalýþýyor mu bunu takip etmem için bu kodu yazýyoruz.17.bit 1 olana kadar demek ama parantez dýþýnda ünlem iþareti ile tersleme iþlemi yapýlýyor.
	//8 000 000 -> 168 000 000 çýkaracak iþlemler;

	RCC -> CR |= 1 << 19; //saat güvenlik sistemi var bu bitte
	//|= iþareti ile ilave iþlemler yapýyoruz.

	//pll açmadan önce pll config ayarlarý yapmamýz gerekiyor.
	/*
	 RCC -> PLLCFGR &= ~ (1 << 0); //PLLM0 0 //bölme oranýnýn 4 olduðu anlamýna geliyor.
	 RCC -> PLLCFGR &= ~ (1 << 1); //PLLM1 0
	 RCC -> PLLCFGR &= ~ (1 << 2); //PLLM2 1
	 RCC -> PLLCFGR &= ~ (1 << 3); //PLLM3 0
	 RCC -> PLLCFGR &= ~ (1 << 4); //PLLM4 0
	 RCC -> PLLCFGR &= ~ (1 << 5); //PLLM5 0

	 Böyle uzun uzun yazmak yerine RCC -> PLLCFGR |=(4 << 0); bu þekilde yazarakta ayný þeyi yapmýþ oluyoruz aslýnda
	  */
	//cubemx programýnda istediðim deðerlere göre bana verilmiþ olan m,n ve p deðerlerini kullandým.
	RCC -> PLLCFGR = 0x00000000; //PLLCFGR Reset 4 lü 4 lü 0 lama iþlemi yapýldý.
	RCC -> PLLCFGR |=(1 << 22); //pll sürücüsünü seçmemizi saðlar biz hangi pll i kullanacaðýz bunu seçeriz(HSE mý yoksa HSI mý kullanacaksýn demek)
	//PLL Source HSE

	RCC -> PLLCFGR |=(4 << 0); // PLL M 4
	RCC -> PLLCFGR |=(168 << 6);// PLL N 168 ayarlamýþ olduk //6 bit sola ötele 168 yazdýr demek
	/*RCC -> PLLCFGR &= ~(1 << 16); // PLL P 2 //16 bit ötele ve 0 yaz demek
	RCC -> PLLCFGR &= ~(1 << 17); // PLL P 2 //17 bit ötele ve 0 yaz demek
    */

	RCC -> CR |= (1 << 24); //pll açma iþlemi yapýlýyor. //PLL ON
	while(!(RCC -> CR & (1 << 25))); // Wait PLL active
	//flag=sistem hazýr olduðunda sistemi kontrol eder. bu bitte flag iþlemi var. her flag iþlemi gördüðümüzde while kullanacaðýz.

	//System Clock is PLL iþlemi
	RCC -> CFGR &= ~ (1 << 0);
	RCC -> CFGR |=  (1 << 1);

	while(!(RCC -> CFGR & (1 << 1))); //sistem clock pll den kullanýlacak demektir. //Select system clock is PLL clock
}


int main(void)
{
	RCC_Config();

	SystemCoreClockUpdate(); //sistem clock'u güncellenmiþ olacak

	systemClock=SystemCoreClock; //systemclock ifadesini systemcoreclock ifadesini atamýþ oluyoruz. //sistem gerçekten 168 MHz olmuþ mu ona bakýyoruz.
	//systemcoreclock sistemimizin çekirdek hýzýdýr. sistem frekansýmýz neyse systemcorclock ta o dur.
	//RCC_DeInit(); //HSIEN PLL OFF sistem 16MHz olarak ayarlanýyor pll yardýmý ile
	//RCC_DeInit(); fonksiyonunu çaðýrdýktan sonra SystemCoreClockUpdate(); dememiz gerekir. bu iþlem bize sistemimizin 16 MHz ile çalýþmasýný saðlar.

  while (1)
  {

  }
}


/*
 * Callback used by stm32f4_discovery_audio_codec.c.
 * Refer to stm32f4_discovery_audio_codec.h for more info.
 */
void EVAL_AUDIO_TransferComplete_CallBack(uint32_t pBuffer, uint32_t Size){
  /* TODO, implement your code here */
  return;
}

/*
 * Callback used by stm324xg_eval_audio_codec.c.
 * Refer to stm324xg_eval_audio_codec.h for more info.
 */
uint16_t EVAL_AUDIO_GetSampleCallBack(void){
  /* TODO, implement your code here */
  return -1;
}
