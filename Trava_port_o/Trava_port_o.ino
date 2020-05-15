#include <Keypad.h> //INCLUSÃO DE BIBLIOTECA
#include <EEPROM.h>

const int buttonPin = 10; 
int buttonState = 0;
int ativo = 0;
unsigned long timer;
const int ledVermelho = 12;
const int ledAmarelo = 13;
const int trava = 11;
String password = "";

const byte qtdLinhas = 4; //QUANTIDADE DE LINHAS DO TECLADO
const byte qtdColunas = 4; //QUANTIDADE DE COLUNAS DO TECLADO
 
//CONSTRUÇÃO DA MATRIZ DE CARACTERES
char matriz_teclas[qtdLinhas][qtdColunas] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};
 
byte PinosqtdLinhas[qtdLinhas] = {9, 8, 7, 6}; //PINOS UTILIZADOS PELAS LINHAS
byte PinosqtdColunas[qtdColunas] = {5, 4, 3, 2}; //PINOS UTILIZADOS PELAS COLUNAS
 
//INICIALIZAÇÃO DO TECLADO
Keypad meuteclado = Keypad( makeKeymap(matriz_teclas), PinosqtdLinhas, PinosqtdColunas, qtdLinhas, qtdColunas); 
 

void setup() {
  Serial.begin(9600);
  Serial.println("Start...");
  pinMode(LED_BUILTIN, OUTPUT);
  
  pinMode(ledVermelho, OUTPUT);
  pinMode(ledAmarelo, OUTPUT);
  pinMode(trava, OUTPUT);
  
  pinMode(buttonPin, INPUT_PULLUP);
  
  carregarPassword();
  
}


void loop() {
  if (digitalRead(buttonPin) == LOW) {
    buttonState = 1;
  } else {
    buttonState = 0;
  }

  if(buttonState == 1) {
    ativar();
  }

  if(ativo == 1) {
    verificaAdmin();
  }

  if (ativo == 1 && (millis() - timer) > 200) {
     desativar();
  }

  char tecla_pressionada = meuteclado.getKey();
  
  if(tecla_pressionada) {
    if(tecla_pressionada == *"*") {
       readPassword();
    }
  }
}

void readPassword() {
  String inputPassword = "";
  digitalWrite(ledAmarelo, HIGH);
  
  Serial.println("Password read...");
  while(true) {
    char tecla_pressionada = meuteclado.getKey();
    
    if(tecla_pressionada == *"#" || digitalRead(buttonPin) == LOW) {
      digitalWrite(ledAmarelo, LOW);
      Serial.println("Exit;");
      return;
    }
    
//    if(tecla_pressionada == *"*") {
//      readPassword();
//      return;
//    }
    
    if (tecla_pressionada) {
      Serial.print("Tecla pressionada : ");
      Serial.println(tecla_pressionada);
      inputPassword = inputPassword + &tecla_pressionada;
      Serial.println(inputPassword);
      digitalWrite(ledAmarelo, LOW);
      delay(200);
      digitalWrite(ledAmarelo, HIGH);

      if(inputPassword.length() == 6) {
        digitalWrite(ledAmarelo, LOW);
        validaPassword(inputPassword);
        return;
      }
    }

  }
}

void validaPassword(String pass) {
   if(pass == password) {
    Serial.println("Password Correto");
    ativar();
   } else {
    Serial.println("Password Errado");
    Serial.println(password);
    digitalWrite(ledVermelho, HIGH);
    delay(1000);
    digitalWrite(ledVermelho, LOW);
   }
}

void ativar() {
  if(ativo == 1) {
    return;
  }

  ativo = 1;
  Serial.println("Ativo");
  digitalWrite(trava, HIGH);
  timer = millis();
}

void desativar() {
  if(ativo == 0) {
    return;
  }

  verificaAdmin();
  
  ativo = 0;
  digitalWrite(trava, LOW);
  Serial.println("Desativado");
}

void verificaAdmin() {
  char tecla_pressionada = meuteclado.getKey();
  if(tecla_pressionada == *"#") {
    desativar();
    definirSenha();
  }
}

void definirSenha() {
  String inputPassword = "";

  digitalWrite(ledAmarelo, HIGH);
  digitalWrite(ledVermelho, HIGH);
  
  Serial.println("Definir senha");
  while(true) {
    char tecla_pressionada = meuteclado.getKey();
    
    if(tecla_pressionada == *"#") {
      digitalWrite(ledAmarelo, LOW);
      digitalWrite(ledVermelho, LOW);
      Serial.println("Exit;");
      return;
    }

    if (tecla_pressionada) {
      Serial.print("Tecla pressionada : ");
      Serial.println(tecla_pressionada);
      inputPassword = inputPassword + tecla_pressionada;
      Serial.println(inputPassword);

      digitalWrite(ledAmarelo, LOW);
      delay(200);
      digitalWrite(ledAmarelo, HIGH);

      if(inputPassword.length() == 6) {
        digitalWrite(ledAmarelo, LOW);
        digitalWrite(ledVermelho, LOW);
        definirPassword(inputPassword);
        return;
      }
    }
  }
}

void definirPassword(String newPassword) {
  for (int i = 0 ; i < EEPROM.length() ; i++) {
    EEPROM.write(i, 0);
  }

  for(int i = 0; i < newPassword.length(); i++){
    EEPROM.write(i, newPassword[i]);
  }
  
  carregarPassword();
}

void carregarPassword() {
  String EEPROMPassword = "";
  
  for(int i = 0; i < 6; i++) {
    byte value = EEPROM.read(i);
    EEPROMPassword = EEPROMPassword + char(value);
  }
  Serial.println(EEPROMPassword);
  
  password = String(EEPROMPassword);
}
