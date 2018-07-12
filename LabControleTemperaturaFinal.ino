  #include <LiquidCrystal.h>
  LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

  #define ki 0.16
  #define kp  9.8  
        
  
  int temperaturaIdeal = 85;
  const int waitPort = 1;
  const int playPort = 5;
  const int maisPort = 3;
  const int menosPort = 4;
  int vwait = 1023;
  int vplay = 1023;
  int vmais = 1023;
  int vmenos = 1023;
  String estado = "OFF";
  int firstRun = 0;

  int integral=0;
  int proporcional=0;
  int pid=0;

  const int LM35 = 0;
  float temperaturaAtual = 0;
  int ADClido = 0;
  int IN1 = 9; //ver se é pwm
  int IN2 = 10;
  int a=0;
  float referencia = 0;
  float temperaturaInicial = 0;
  float erro = 0;
  
  void setup() {
    Serial.begin(9600);
    analogReference(INTERNAL);
    lcd.begin(16, 2); 
    pinMode(IN1,OUTPUT);
    pinMode(IN2,OUTPUT);
  }
  
  
  void loop() { 
    ADClido = analogRead(LM35);
    temperaturaAtual = ADClido * 0.1075268817204301;
      
    if(firstRun == 0){
      onCreate(); 
      setting(); 
    }
    
    firstRun = 1;
    //vplay = analogRead(playPort);
    //vwait = analogRead(waitPort);
    
    if(vwait!=1023){
      temperaturaInicial = temperaturaAtual;
      setting();
    }
    
    controle();
    cooler();
    atualizaLCD();

    Serial.print(temperaturaAtual);
    Serial.print("\r\n");
    delay(300);
  }

  
  
  void onCreate(){
    temperaturaInicial = temperaturaAtual;
    firstRun = 1;
    lcd.setCursor(0,0);
    lcd.print("T.Ideal= "); 
    lcd.print(temperaturaIdeal);
    lcd.setCursor(12,0);
    lcd.print(estado); 
    lcd.setCursor(0,1);
    lcd.print("T.Atual= "); 
    lcd.print(temperaturaAtual); 
  }
  
  void setting(){
    estado = "OFF";
    lcd.setCursor(12,0);
    lcd.print(estado);
    while(vplay==1023){
      vplay = analogRead(playPort);
      vmais = analogRead(maisPort);
      vmenos = analogRead(menosPort);
      
      if (vmais!=1023){
        if(temperaturaIdeal < 90){
          temperaturaIdeal = temperaturaIdeal + 1;
        } 
      }
      else if (vmenos!=1023){
        if(temperaturaIdeal > 80){
          temperaturaIdeal = temperaturaIdeal - 1;
        } 
      }
      lcd.setCursor(0,0);
      lcd.print("T.Ideal= "); 
      lcd.print(temperaturaIdeal);
      delay(300);
      lcd.setCursor(0,0);
      lcd.print("T.Ideal= "); 
      lcd.print("  ");
      delay(300);
    }
    referencia = temperaturaInicial - temperaturaIdeal;
  }

  void controle(){
    erro = referencia + 1*(temperaturaAtual - temperaturaInicial);
    
    proporcional = erro;
    if(proporcional*kp > 255){proporcional = (255/kp);}
    
    integral += proporcional;
    if(integral*ki > 180){integral = (180/ki);}
    
    pid = proporcional*kp + integral*ki;
    
    if(pid>255) {(pid = 255);}
    else if(pid<10) {(pid = 0);}
  }

  void cooler()
  {
    analogWrite(IN1,pid);
    analogWrite(IN2,0);
  }

  void atualizaLCD(){
    lcd.setCursor(0,0);
    lcd.print("T.Ideal= "); 
    lcd.print(temperaturaIdeal);
    estado = "ON ";
    lcd.setCursor(12,0);
    lcd.print(estado); 
    lcd.setCursor(0,1);
    lcd.print("T.Atual= "); 
    lcd.print(temperaturaAtual); 
  }
