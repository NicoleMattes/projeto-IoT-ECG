import processing.serial.*;
 
Serial myPort;        // A porta serial
int xPos = 1;         // posição horizontal do gráfico
float height_old = 0;
float height_new = 0;
float inByte = 0;
int BPM = 0;
int beat_old = 0;
float[] beats = new float[500];  // Usado para calcular o BPM médio
int beatIndex;
float threshold = 620.0;  // Limite em que ocorre o cálculo de BPM
boolean belowThreshold = true;
PFont font;
 
void setup () {
  // definir o tamanho da janela:
  size(1000, 400);        
 
  // Lista todas as portas seriais disponíveis
  println(Serial.list());
  // Abra qualquer porta que você está usando.
  myPort = new Serial(this, Serial.list()[2], 9600);
  // não gere um serialEvent () a menos que você obtenha um caractere de nova linha:
  myPort.bufferUntil('\n');
  // definir fundo inicial:
  background(0xff);
  font = createFont("Ariel", 12, true);
}
 
 
void draw () {
     // Mapeie e desenhe a linha para o novo ponto de dados
     inByte = map(inByte, 0, 1023, 0, height);
     height_new = height - inByte; 
     line(xPos - 1, height_old, xPos, height_new);
     height_old = height_new;
    
      // na borda da tela, volte ao início:
      if (xPos >= width) {
        xPos = 0;
        background(0xff);
      } 
      else {
        // incrementa a posição horizontal:
        xPos++;
      }
      
      // desenhe texto para BPM periodicamente
      if (millis() % 128 == 0){
        fill(0xFF);
        rect(0, 0, 200, 20);
        fill(0x00);
        text("BPM: " + inByte, 15, 10);
      }
}
  
void serialEvent (Serial myPort) 
{
  // obtenha a string ASCII:
  String inString = myPort.readStringUntil('\n');
 
  if (inString != null) 
  {
    // apare qualquer espaço em branco:
    inString = trim(inString);
 
    // Se a detecção de derivações for verdadeira, notifique com linha azul
    if (inString.equals("!")) 
    { 
      stroke(0, 0, 0xff); // Defina o curso para azul (R, G, B)
      inByte = 512;  // meio do intervalo ADC (linha plana)
    }
    // Se os dados são bons, deixe-os passar
    else 
    {
      stroke(0xff, 0, 0); //Set stroke to red ( R, G, B)
      inByte = float(inString); 
      
      // verificação de cálculo de BPM
      if (inByte > threshold && belowThreshold == true)
      {
        calculateBPM();
        belowThreshold = false;
      }
      else if(inByte < threshold)
      {
        belowThreshold = true;
      }
    }
  }
}
  
void calculateBPM () 
{  
  int beat_new = millis();    // obtém o milissegundo atual
  int diff = beat_new - beat_old;    // encontre o tempo entre as duas últimas batidas
  float currentBPM = 60000 / diff;    // converter em batidas por minuto
  beats[beatIndex] = currentBPM;  // armazene em array para converter a média
  float total = 0.0;
  for (int i = 0; i < 500; i++){
    total += beats[i];
  }
  BPM = int(total / 500);
  beat_old = beat_new;
  beatIndex = (beatIndex + 1) % 500;  // percorre a matriz em vez de usar a fila FIFO
}