#include "stm32f4xx.h"
#include "stm32f4_discovery.h"

//Reference manual diye d�k�man�m�z var ve bu d�k�man bize stm32f4xx kart�n� nas�l kodlayaca��m�z� s�yl�yor.


extern uint32_t SystemCoreClock;
//bu kodun anlam� system_stm32f4xx dosyas� i�erindesindeki tan�mlam�� oldu�um systemcoreclock ifadesini dosyama �a��r�yorum.(168MHz olarak tan�mlad�m)
//extern ediyorsak ilk de�erini vermeyiz

uint32_t systemClock; //benim kendimin takip edece�i bir de�i�ken tan�mlad�m.

//register=kaydedici demek yani biz bir mikrodenetleyiciye belli i�lemler yapt�rmak istiyorsak
//�ncelikle o mikrodenetleyicinin o i�lemi yapabilecek noktalar�na belirli ayarlar yapmam�z laz�m


//t�m �evresel birimlerde kontrol register ad� alt�nda bir register vard�r. Kontrol etmek i�in kullan�l�r.
//RCC=RESET AND CLOCK CONTROL anlam�na gelmektedir.
//bu bi register(kaydedici) bunun i�inde belli bitler vard�r.Stm32f4xx kart� 32 bitliktir yani kaydedicilerinde 32 tane bitler var demektir.

void RCC_Config(void){ //yap�lmas� gereken ilk ad�md�r.��nk�;

	RCC -> CR &= ~ (1 << 0);//ayr�ca 0x00000083 �eklinde de yazabiliriz ayn� adresi ifade eder.
	//HSI OFF, HSI=dahili osilat�rler. Dahili osilat�rler harici osilat�rlere g�re daha kalitesizdir. �zellikle kritik uygulamalarda �nemli bu durum
	//kontrol register�m resetlendi demek bu kod ile
	//&= bu i�aret kaydet demek
	//~ bu i�aret tersle demek.Bu �ekilde (&= ~) kullan�l�rsa ise sadece 0.biti tersle demek
	RCC -> CR |= 1 << 16; //HSE ON HSE=harici osilat�rler. //16.bite 1 yazmam gerekiyor ve bu �ekilde sadece 16.bite yazm�� oluyoruz.
	//|= bu i�aret daha �nceki kulland���m�z &= bu i�arete +1 ekler. ya da e�ittir yapar
	while(!(RCC -> CR & (1 << 17))); // Wait HSE active
	//HSE �al���yor mu bunu takip etmem i�in bu kodu yaz�yoruz.17.bit 1 olana kadar demek ama parantez d���nda �nlem i�areti ile tersleme i�lemi yap�l�yor.
	//8 000 000 -> 168 000 000 ��karacak i�lemler;

	RCC -> CR |= 1 << 19; //saat g�venlik sistemi var bu bitte
	//|= i�areti ile ilave i�lemler yap�yoruz.

	//pll a�madan �nce pll config ayarlar� yapmam�z gerekiyor.
	/*
	 RCC -> PLLCFGR &= ~ (1 << 0); //PLLM0 0 //b�lme oran�n�n 4 oldu�u anlam�na geliyor.
	 RCC -> PLLCFGR &= ~ (1 << 1); //PLLM1 0
	 RCC -> PLLCFGR &= ~ (1 << 2); //PLLM2 1
	 RCC -> PLLCFGR &= ~ (1 << 3); //PLLM3 0
	 RCC -> PLLCFGR &= ~ (1 << 4); //PLLM4 0
	 RCC -> PLLCFGR &= ~ (1 << 5); //PLLM5 0

	 B�yle uzun uzun yazmak yerine RCC -> PLLCFGR |=(4 << 0); bu �ekilde yazarakta ayn� �eyi yapm�� oluyoruz asl�nda
	  */
	//cubemx program�nda istedi�im de�erlere g�re bana verilmi� olan m,n ve p de�erlerini kulland�m.
	RCC -> PLLCFGR = 0x00000000; //PLLCFGR Reset 4 l� 4 l� 0 lama i�lemi yap�ld�.
	RCC -> PLLCFGR |=(1 << 22); //pll s�r�c�s�n� se�memizi sa�lar biz hangi pll i kullanaca��z bunu se�eriz(HSE m� yoksa HSI m� kullanacaks�n demek)
	//PLL Source HSE

	RCC -> PLLCFGR |=(4 << 0); // PLL M 4
	RCC -> PLLCFGR |=(168 << 6);// PLL N 168 ayarlam�� olduk //6 bit sola �tele 168 yazd�r demek
	/*RCC -> PLLCFGR &= ~(1 << 16); // PLL P 2 //16 bit �tele ve 0 yaz demek
	RCC -> PLLCFGR &= ~(1 << 17); // PLL P 2 //17 bit �tele ve 0 yaz demek
    */

	RCC -> CR |= (1 << 24); //pll a�ma i�lemi yap�l�yor. //PLL ON
	while(!(RCC -> CR & (1 << 25))); // Wait PLL active
	//flag=sistem haz�r oldu�unda sistemi kontrol eder. bu bitte flag i�lemi var. her flag i�lemi g�rd���m�zde while kullanaca��z.

	//System Clock is PLL i�lemi
	RCC -> CFGR &= ~ (1 << 0);
	RCC -> CFGR |=  (1 << 1);

	while(!(RCC -> CFGR & (1 << 1))); //sistem clock pll den kullan�lacak demektir. //Select system clock is PLL clock
}


int main(void)
{
	RCC_Config();

	SystemCoreClockUpdate(); //sistem clock'u g�ncellenmi� olacak

	systemClock=SystemCoreClock; //systemclock ifadesini systemcoreclock ifadesini atam�� oluyoruz. //sistem ger�ekten 168 MHz olmu� mu ona bak�yoruz.
	//systemcoreclock sistemimizin �ekirdek h�z�d�r. sistem frekans�m�z neyse systemcorclock ta o dur.
	//RCC_DeInit(); //HSIEN PLL OFF sistem 16MHz olarak ayarlan�yor pll yard�m� ile
	//RCC_DeInit(); fonksiyonunu �a��rd�ktan sonra SystemCoreClockUpdate(); dememiz gerekir. bu i�lem bize sistemimizin 16 MHz ile �al��mas�n� sa�lar.

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
