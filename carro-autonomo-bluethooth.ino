#include <SoftwareSerial.h>
#include <Servo.h>

/*
  ------------------------------------------Robô Autônomo SR 2------------------------------------------------
  Versão do Software: 1.0 SR 2 //não apague essa linha, para futuras consultas
  Software open source, favor citar o autor: Seu Robô https://SeuRobo.com.br/
  Criado por: Leonardo Campbell - Seu Robô  Uptades:  Erteza Tawsif Efaz
  Robô autonomo SR 2 é uma versão bem melhorada do Robô SR 1
*/

//Definindo os pinos
#define trigPin A1          // Pino TRIG do sensor no pino analógico A0
#define echoPin A2          // Pino ECHO do sensor no pino analógico A1
// motor um                 // Ligação dos pinos da Ponte H L298N
#define in1  7              // Pino in1 na porta digital 7
#define in2  6              // Pino in2 na porta digital 6
// motor dois               // Ligação dos pinos da Ponte H L298N
#define in3  5              // Pino in3 na porta digital 5
#define in4  4              // Pino in4 na porta digital 4
#define buzzer 10           // Pino buzzer na porta 10
#define tempo 10            // Tempo entre cada toque de cada frequencia
#define seta 13             // Pino seta na porta 13

int frequencia = 0;
Servo servoSensor;          // Crie um objeto Servo para controlar o Servo.
SoftwareSerial ble(2, 3);   // Cria um objeto bluethooth ligando rx-3 e tx-2
int incomingByte;

//função para procurar obtasculo a todo o tempo
int Procurar (void) {
  float duracao = 0.0;              // variavael para quartar a duração do retorno do som
  float CM = 0.0;                   // variavael para quartar a distancia
  digitalWrite(trigPin, LOW);       //não envia som
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);      //envia som
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);       //não envia o som e espera o retorno do som enviado
  duracao = pulseIn(echoPin, HIGH); //Captura a duração em tempo do retorno do som.
  CM = (duracao / 58.8);            //Calcula a distância em centimetros
  Serial.print("Distancia em CM: ");
  Serial.println(CM);               //Imprimi no monitor serial a distancia
  return CM;                        // Return to CM.
}

//Variaveis
int DistanciaDireita, DistanciaEsquerda;  // variavel de Distâncias de ambos os lados
float Distancia = 0.00;                   // variavel para guardar a distancia

// Função que é executado na inicialização do Arduino
void setup() {
  Serial.begin(9600); // inicializa a comunicação serial para mostrar dados
  ble.begin(9600);

  //Define o servo na porta 11
  servoSensor.attach(11);

  // Definir todos os pinos de controle do motor como saídas
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  pinMode(in3, OUTPUT);
  pinMode(in4, OUTPUT);
  pinMode(buzzer, OUTPUT);
  pinMode(setaDoFioVermelho, OUTPUT);

  //Configuraçõs do sensor ultrassonico
  pinMode(trigPin, OUTPUT);     //define o pino TRIG como saída
  pinMode(echoPin, INPUT);      //define o pino ECHO como entrada
}

// Função principal do Arduino
void loop() {
  if (ble.available() > 0) {
    // read the oldest byte in the serial buffer:
    incomingByte = ble.read();
    if (incomingByte == 's' || incomingByte == 'S') {
      ble.println("Parando carrinho...");
      digitalWrite(setaDoFioVermelho, HIGH);
      servoSensor.detach();
      tocaAlarme();
      Parar ();
    } else if (incomingByte == 'f' || incomingByte == 'F')  {
      ble.println("Ligando carrinho...");
      digitalWrite(setaDoFioVermelho, LOW);
      noTone(buzzer);
      servoSensor.attach(11);
    }
  }

  servoSensor.write (90);                           // Gira o Servo com o sensor a 90 graus
  delay (100);                                      // Aguarda 100 milesugodos
  Distancia = Procurar ();                          // Medindo a Distancia em CM.
  if (Distancia < 40) {                             // Se há obstáculo encontrado a menos de 40cm.
    direcao ();                                      // Se Frente estiver bloqueado, mude de direção
  } else if (Distancia >= 40)  {                      // Se o obstáculo for encontrado entre a mais de 40cm
    Frente ();                                      // Robô se move para a direção da Frente.
  }

}

// Função para pegar as distancias de cada direção
void direcao () {
  Parar ();                                         // O robô Para
  ParaTras ();                                      // O robô vai para tras
  Parar ();                                         // O robô Para
  servoSensor.write (180);                          // Gira o Servo com o sensor a 180 graus
  delay (1000);
  DistanciaEsquerda = Procurar ();                  // Defina a Distancia da Esquerda
  delay (500);
  servoSensor.write (0);                            // Gira o Servo com o sensor a 0 graus
  delay (500);
  DistanciaDireita = Procurar ();                   // Defina a Distancia da Direita
  delay (500);
  servoSensor.write (90);                           // Gira o Servo com o sensor a 90 graus
  delay (500);
  CompareDistance ();                               // Encontre a distância mais longa.
}

// Função para calcular qual a distancia é melhor para o robô ir
void CompareDistance () {
  if (DistanciaDireita > DistanciaEsquerda) {       // Se a direita está menos obstruída.
    Vireadireita ();                                // O robô vai virar a direita
  }
  else if (DistanciaEsquerda > DistanciaDireita) {  // Se Esquerda estiver menos obstruída.
    VireaEsquerda ();                               // Robô Vire na direção esquerda.
  }
  else {                                            // Se ambos estiverem igualmente obstruídos.
    Retorne ();                                     // Robô Vire-se.
  }
}

void tocaAlarme() {
  for (frequencia = 150; frequencia < 1800; frequencia += 1) { // Tone que produz sirene de polícia
    tone(buzzer, frequencia, tempo);
    delay(3);
  }
  for (frequencia = 1800; frequencia > 150; frequencia -= 1) { // Tone que produz sirene de polícia
    tone(buzzer, frequencia, tempo);
    delay(3);
  }
}

// Função para fazer o carro parar
void Parar()
{
  Serial.println("Robô: Parar ");
  digitalWrite(in1, LOW);                           //Configurar a ponte h
  digitalWrite(in2, LOW);
  digitalWrite(in3, LOW);
  digitalWrite(in4, LOW);
  delay(600);                                       //aguarda um tempo
}

// Função para fazer o robô andar para frente
void Frente()
{
  Serial.println("Robô: Frente ");
  digitalWrite(in1, HIGH);                          //Configurar a ponte h
  digitalWrite(in2, LOW);
  digitalWrite(in3, HIGH);
  digitalWrite(in4, LOW);
}

// Função que faz o robô andar para trás e emite som quando ele dá ré
void ParaTras()
{
  Serial.println("Robô: Ré ");
  digitalWrite(in1, LOW);                           //Configurar a ponte h
  digitalWrite(in2, HIGH);
  digitalWrite(in3, LOW);
  digitalWrite(in4, HIGH);
  delay(300);                                       //aguarda um tempo
}

// Função que faz o robô virar à direita, https://SeuRobo.com.br/
void Vireadireita()
{
  Serial.println("Robô: Direita ");
  digitalWrite(in1, LOW);                           //Configurar a ponte h
  digitalWrite(in2, HIGH);
  digitalWrite(in3, HIGH);
  digitalWrite(in4, LOW);
  delay(300);                                       //aguarda um tempo
}

// Função que faz o robô virar à esquerda
void VireaEsquerda()
{
  Serial.println("Robô: Esquerda ");
  digitalWrite(in1, HIGH);                          //Configurar a ponte h
  digitalWrite(in2, LOW);
  digitalWrite(in3, HIGH);
  digitalWrite(in4, LOW);
  delay(300);
}

//Função para o robô virar para o outro lado
void Retorne () {
  Serial.println("Robô: Girar ");
  digitalWrite(in1, HIGH);                          //Configurar a ponte h
  digitalWrite(in2, LOW);
  digitalWrite(in3, HIGH);
  digitalWrite(in4, LOW);
  delay (700);                                      //aguarda um tempo
}

//Fim
//Versão do Software: 1.0 SR 2 SeuRobo.com.br //não apague essa linha, para futuras consultas
