#include <Wire.h>
#include <LiquidCrystal_I2C.h>

/*
 * **********************
 * **********************
 * ****** Ayarlar *******
 * **********************
 * **********************
 */



#define TEKRARLA 100                      // Testin kaç defa tekrarlanacağını belirler
#define STANDART_BEKLEME 3000             // Strandart olarak rölenin kaç saniyede bir açılacağını belirler. Sonradan değiştirilebilir.
#define TEKRAR_ACILMA 2000                // Cihazın kapandıktan sonra tekrar açılması için gereken süreyi belirler.
int LDR_HASSASIYET_ARALIGI[2] = {0,1000}; // LDR'nin hassasiyetinin hangi değerler arasında olacağını belirler.
int LDR_ON_SABIT_ARALIGI = 16;            // LDR fiziksel olarak makaron ile kısıtlandığında 0-1000 hassasiyetinde ışıkta 988 civarı bir değişim olur.
                                          // Aralığı sağlayabilmek için ışıkta 12 birimlik değişimin bu değişkene belirtilmesi gerekiyor.

/*
 * **********************
 * **********************
 * **********************
 * **********************
 * **********************
 */


/*
 * ------------------------------------
 * ------------------------------------
 * --- Bağlantının yapılacağı bölüm ---
 * ------------------------------------
 * ------------------------------------
 */

#define baslat_butonu 3
#define role 5
#define buzzer 11
#define ldr A0
#define sureAyar A7
#define sure_ldr_AyarKontrolButon 7

/*
 * ------------------------------------
 * ------------------------------------
 */

int sensorDegeri = 0,
    sure = 0,
    bekle = STANDART_BEKLEME,
    ldrOku = 0,
    ldrAyar = (int)LDR_HASSASIYET_ARALIGI[1] - LDR_ON_SABIT_ARALIGI;

char sure_ldr_AyarKontrol = 0; // 0 -> Boş
                               // 1 -> süre
                               // 2 -> LDR

boolean baslat = false,
        ayar = false,
        kontrol_yontemi = true, //LDR ile mi yoksa gözle mi kontrol yapılacağını belirler. false->Gözle kontrol, true->LDR ile kontrol
        teste_basla = false;



LiquidCrystal_I2C lcd(0x3F,16,2);


void hata (char tekrarla = 5){
  for(char i = 0; i < tekrarla; i++){
    analogWrite(buzzer, 100);
    delay(100);
    analogWrite(buzzer, 0);
    delay(180);
  }
}

void bitti (char tekrarla = 2){
  for(char i = 0; i < tekrarla; i++){
    analogWrite(buzzer, 50);
    delay(300);
    analogWrite(buzzer, 0);
    delay(600);
  }
}



void setup() {
  Serial.begin(9600);
  lcd.init();
  lcd.backlight();

  lcd.setCursor(2,0);
  lcd.print("Dynamix TCP");
  lcd.setCursor(0,1);
  lcd.print("Kontrol Sistemi");

  delay(1500);
  lcd.clear();


  pinMode(role, OUTPUT);
  pinMode(buzzer, OUTPUT);
  pinMode(baslat_butonu, INPUT);
  pinMode(sure_ldr_AyarKontrolButon, INPUT);

  digitalWrite(role, HIGH);
  digitalWrite(buzzer, LOW);



}

void loop() {

  sensorDegeri = analogRead(ldr);
  sure = map(analogRead(sureAyar),0,1023,3000,8000);
  baslat = digitalRead(baslat_butonu);
  ayar = digitalRead(sure_ldr_AyarKontrolButon);
  ldrOku = map(analogRead(ldr),0,1023,LDR_HASSASIYET_ARALIGI[0],LDR_HASSASIYET_ARALIGI[1]);


  switch(sure_ldr_AyarKontrol){
    case 0:
      lcd.setCursor(1,0);
      lcd.print("Baslatmak Icin");
      lcd.setCursor(0,1);
      lcd.print("Sag Butona Basin");
    break;
    case 1:
      lcd.setCursor(2,0);
      lcd.print("Delay Ayari");
      lcd.setCursor(2,1);
      lcd.print("Sure: ");
      lcd.print(sure);
      lcd.print("ms");
      bekle = sure;
    break;
    case 2:
      ldrAyar = (int)LDR_HASSASIYET_ARALIGI[1] - map(analogRead(sureAyar),0,1023,0,100);
      lcd.setCursor(1,0);
      lcd.print("LDR Hassasiyeti");
      lcd.setCursor(2,1);
      lcd.print(ldrAyar);
      Serial.println(LDR_HASSASIYET_ARALIGI[1]);
      Serial.println(map(analogRead(sureAyar),0,1023,0,100));
      
    break;
    case 3:
      if(kontrol_yontemi == true){
        lcd.setCursor(0,0);
        lcd.print("Kntrl Yntm Scnz");
        lcd.setCursor(0,1);
        lcd.print(" >ACIK< KAPALI ");
      }else{
        lcd.setCursor(0,0);
        lcd.print("Kntrl Yntm Scnz");
        lcd.setCursor(0,1);
        lcd.print(" ACIK >KAPALI< ");
      }
      if(baslat == HIGH){
        lcd.clear();
        kontrol_yontemi =! kontrol_yontemi;
        while(baslat) {
          baslat = digitalRead(baslat_butonu);
          if(baslat == LOW) break;
        }
      }


    break;
    case 4:
      lcd.setCursor(3,0);
      lcd.print("LDR Degeri");
      lcd.setCursor(6,1);
      lcd.print(ldrOku);
      delay(50);
    break;
    default:
      sure_ldr_AyarKontrol++;
    break;
  }
  if(teste_basla == true){
    for(int kontEt=1;kontEt<=TEKRARLA;kontEt++){
      lcd.clear();
      lcd.setCursor(2,0);
      lcd.print("Kontrol");
      lcd.setCursor(0,1);
      lcd.print(kontEt);

      if(ayar == HIGH){
        break;
      }

      digitalWrite(role, LOW);
      delay(TEKRAR_ACILMA);
      ldrOku = map(analogRead(ldr),0,1023,LDR_HASSASIYET_ARALIGI[0],LDR_HASSASIYET_ARALIGI[1]);
      Serial.print("LDR: ");
      Serial.println(ldrOku);


      if(kontEt == TEKRARLA){
        sure_ldr_AyarKontrol = 0;
        
        lcd.clear();
        lcd.setCursor(1,0);
        lcd.print("Test Basarili");
        lcd.setCursor(0,1);
        lcd.print("Snrki Cihza Gcnz");

        Serial.println();
        Serial.println("Cihaz saglam. 100 defa test edildi.");
        Serial.print("Delay Suresi\t\t:");
        Serial.println(bekle);
        Serial.print("LDR Hassasiyet\t\t:");
        Serial.println(ldrAyar);
        Serial.print("LDR ile mi manual mi\t:");
        if(kontrol_yontemi == true)
          Serial.println("LDR");
        else
          Serial.println("MANUAL");

        bitti(1);
        teste_basla = false;
        digitalWrite(role, HIGH);
        break;
      }


    if(kontrol_yontemi == true && (ldrOku > ldrAyar)){
        hata(3);
        lcd.clear();
        lcd.setCursor(1,0);
        lcd.print(kontEt);
        lcd.print(". denemede ");
        lcd.setCursor(0,1);
        lcd.print("hata yasandi!");
        delay(5000);
        teste_basla = false;
        sure_ldr_AyarKontrol = 0;
        lcd.clear();
        Serial.println();
        Serial.print(kontEt);
        Serial.println(". Denemede hata yasandi!");
        Serial.println();
        Serial.print("Delay Suresi\t\t: ");
        Serial.println(bekle);
        Serial.print("LDR Hassasiyeti\t\t: ");
        Serial.println(ldrAyar);
        Serial.print("LDR ile mi manual mi\t: ");
        digitalWrite(role, HIGH);
        if(kontrol_yontemi == true)
          Serial.println("LDR");
        else
          Serial.println("MANUAL");
        break;
    }

      digitalWrite(role, HIGH);
      delay(bekle);
    }
  }

  if(baslat == HIGH && sure_ldr_AyarKontrol != 3){

    // Başlat butonuna basınca kodlar burada çalışacak..
    lcd.clear();
    teste_basla = true;

    while(baslat) {
      baslat = digitalRead(baslat_butonu);
      if(baslat == LOW) break;
    }
  }


  if(ayar == HIGH){


    // Ayar butonuna basınca kodlar burada çalışacak..
    sure_ldr_AyarKontrol++;
    lcd.clear();



    while(ayar) {
      ayar = digitalRead(sure_ldr_AyarKontrolButon);
      if(ayar == LOW) break;
    }
  }

}
