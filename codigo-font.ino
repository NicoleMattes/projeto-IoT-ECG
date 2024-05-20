void setup() {
  Serial.begin(9600);
  pinMode(10, INPUT); // Configuração para detecção de derivações LO +
  pinMode(11, INPUT); // Configuração para detecção de leads off LO -
}

void loop() {
  if ((digitalRead(10) == HIGH) || (digitalRead(11) == HIGH)) {
    Serial.println('!');
  } else {
    // Envia o valor da entrada analógica 0
    Serial.println(analogRead(A0));
  }
  // Espere um pouco para evitar que os dados seriais saturem
  delay(1);
}
