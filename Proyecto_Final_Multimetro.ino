#include <SoftwareSerial.h>

const int MED1 = 7;         // Pin positivo
int Valuable;                // Variable que almacena el valor raw (0 a 1023)
float VOLTAGE;               // Variable que almacena el voltaje (0.0 a 25.0)

float Sensibilidad = 0.185;

const int pinAnalogico = A0;    // Pin analógico para medir la tensión
const float resistenciaConocida = 1000.0; // Resistencia conocida en ohmios (1k ohm)

SoftwareSerial BLUEE(2, 3);

unsigned long tiempoAnterior = 0;
const unsigned long intervaloMedicion = 1000; // Intervalo de medición en milisegundos

int PinA0 = A2;
int lectura = 0;
int Ve = 5; // Tensión en el Arduino.
float VR2 = 0;
float R1 = 10000;
float R2 = 0;
float I = 0;
float relacion = 0;

char recep;
int estadoActual = 0; // 0: Esperando, 1: Medir voltaje, 2: Medir resistencia, 3: Medir corriente

void setup() {
  pinMode(MED1, OUTPUT);  // Configurar el pin positivo como salida
  Serial.begin(9600);    // Iniciar la comunicación serial
  BLUEE.begin(9600);
}

void loop() {
  // Verificar si hay datos disponibles desde la aplicación Bluetooth
  if (BLUEE.available()) {
    recep = BLUEE.read();
    Serial.println(recep);

    // Cambiar el estado actual según el comando recibido
    if (recep == '1') {
      estadoActual = 1;
    }
    else if (recep == '2') {
      estadoActual = 2;
    }
    else if (recep == '3') {
      estadoActual = 3;
    }
  }

  // Realizar mediciones continuas a intervalos regulares
  unsigned long tiempoActual = millis();
  if (tiempoActual - tiempoAnterior >= intervaloMedicion) {
    tiempoAnterior = tiempoActual;

    // Realizar la medición según el estado actual
    switch (estadoActual) {
      case 1:
        medirVoltaje();
        break;
      case 2:
        medirResistencia();
        break;
      case 3:
        medirCorriente();
        break;
      default:
        // Estado por defecto: ESPERANDO
        break;
    }
  }
}

void medirVoltaje() {
  digitalWrite(MED1, HIGH);
  float voltaje =  (float)25*analogRead(A1)/1023;
  Serial.print("Voltaje: ");
  Serial.println(voltaje);          // Mostrar el valor por serial
  BLUEE.print(voltaje);
  BLUEE.print(";");
}

void medirResistencia() {
  digitalWrite(MED1, LOW);
  lectura = analogRead(PinA0);
  if (lectura) {
    relacion = lectura * Ve;
    VR2 = (relacion) / 1024.0;
    relacion = (Ve / VR2) - 1;
    R2 = R1 * relacion;
    I = Ve / (R1 + R2) * 1000;
  }
  Serial.print("R2: ");
  Serial.println(R2);
  BLUEE.print(R2);
  BLUEE.print(";");
}

void medirCorriente() {
  float Corriente = calculo(500);
  Serial.print("Corriente: ");
  Serial.println(Corriente, 3);
  BLUEE.print(Corriente, 3);
  BLUEE.print(";");
}

float calculo(int numeroMuestral) {
  float LeerSenso = 0;
  float inten = 0;
  for (int i = 0; i < numeroMuestral; i++) {
    LeerSenso = analogRead(A2) * (5.0 / 1023.0);
    inten = inten + (LeerSenso - 2.5) / Sensibilidad;
  }
  inten = inten / numeroMuestral;
  return (inten);
}
