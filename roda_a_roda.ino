#include <LiquidCrystal.h>

#include <math.h>

#include <ctype.h>

#define RS 12
#define EN 11
#define D4 5
#define D5 4
#define D6 3
#define D7 2
#define QUANTIDADE_DE_PALAVRAS 4
#define TAM_MAX_DE_LETRAS 47
#define BUZZER 13
#define FREQUENCIA_BUZZER 1000
#define DELAY_BUZZER 100
#define BOTAO 10

LiquidCrystal lcd(RS, EN, D4, D5, D6, D7);

const String PALAVRAS[QUANTIDADE_DE_PALAVRAS] = {
    "Amarelo",
    "Verde",
    "Vermelho",
    "Azul"
};

int indice_do_sorteio = -1;
// const int INDICE_SORTEADO = random(QUANTIDADE_DE_PALAVRAS);
String palavra_sorteada;
int quantidade_de_letras;

char palavra_no_display[TAM_MAX_DE_LETRAS];

int tentativas_restantes;
bool vitoria;

int estado_do_botao;
int estado_anterior_do_botao = 0;

int estado_do_botao_dentro_do_loop;
int estado_anterior_do_botao_dentro_do_loop = 0;
int gameloop = 1;

void mostrar_mensagem_inicial();
void ocultar_palavra_do_display(char palavra_no_display[TAM_MAX_DE_LETRAS]);
bool alterar_palavra_no_display(char letra, int indice);
int procurar_letra_na_palavra_sorteada(char letra);
void finalizar_jogo_como_derrota();
void finalizar_jogo_como_vitoria();
void verificar_se_palavra_foi_descoberta();
void reiniciar_jogo();
void reiniciar_variaveis();
void sortear_outra_palavra();
void rodar_display();

void setup() {
    Serial.begin(9600);
    pinMode(BUZZER, OUTPUT);
    pinMode(BOTAO, INPUT);
    lcd.begin(16, 2);
}

void loop() {
    estado_do_botao = digitalRead(BOTAO);

    if (estado_do_botao == LOW && estado_anterior_do_botao == HIGH) {
        reiniciar_jogo();
    }

    estado_anterior_do_botao = digitalRead(BOTAO);
}

void mostrar_mensagem_inicial() {
    String mensagem = "RODA A RODA";
    int tamanho = mensagem.length();

    lcd.setCursor(15, 0);
    lcd.print(mensagem);

    for (int i = 0; i < (15 + tamanho); i++) {
        lcd.scrollDisplayLeft();
        delay(200);
    }

    lcd.clear();
}

void ocultar_palavra_do_display(char palavra_no_display[TAM_MAX_DE_LETRAS]) {
    int i;
    for (i = 0; i < quantidade_de_letras; i++) {
        if (palavra_sorteada[i] == ' ') {
            palavra_no_display[i] = ' ';
        } else {
            palavra_no_display[i] = '_';
        }
    }

    palavra_no_display[i] = '\0';
}

bool alterar_palavra_no_display(char letra, int indice) {
    int i;
    for (i = indice; i < quantidade_de_letras; i++) {
        if (tolower(letra) == tolower(palavra_sorteada[i])) {
            palavra_no_display[i] = palavra_sorteada[i];
        }

    }

    lcd.clear();
    lcd.print(palavra_no_display);
}

int procurar_letra_na_palavra_sorteada(char letra) {
    int i;

    for (int i = 0; i < quantidade_de_letras; i++) {
        if (tolower(letra) == tolower(palavra_sorteada[i])) {
            return i;
        }
    }

    return -1;
}

void finalizar_jogo_como_derrota() {
    lcd.clear();
    lcd.print("Voce perdeu!");
    lcd.setCursor(0, 1);
    lcd.print("\"");
    lcd.print(palavra_sorteada);
    lcd.print("\"");

    tone(BUZZER, 1000);
    gameloop = 0;
}

void finalizar_jogo_como_vitoria() {
    lcd.clear();
    lcd.print("Voce ganhou!");
    lcd.setCursor(0, 1);
    lcd.print("Parabens!");

    gameloop = 0;
}

void verificar_se_palavra_foi_descoberta() {
    bool foi_descoberta = true;

    for (int i = 0; i < quantidade_de_letras; i++) {
        if (palavra_no_display[i] == '_') {
            foi_descoberta = false;
            break;
        }
    }

    if (foi_descoberta) {
        vitoria = true;
    }
}

void reiniciar_jogo() {
    reiniciar_variaveis();

    while (gameloop == 1) {
        int estado_do_botao_dentro_do_loop = digitalRead(BOTAO);

        if (estado_do_botao_dentro_do_loop == LOW && estado_anterior_do_botao_dentro_do_loop == HIGH) {
            break;
        }

        estado_anterior_do_botao_dentro_do_loop = digitalRead(BOTAO);

        while (Serial.available() > 0) {
          rodar_display();
          lcd.print(palavra_no_display);
            char letra_palpite = (char)(Serial.read());
            int achou_letra = procurar_letra_na_palavra_sorteada(letra_palpite);

            if (achou_letra != -1) {

                alterar_palavra_no_display(letra_palpite, achou_letra);
            } else {
                tentativas_restantes--;
                tone(BUZZER, FREQUENCIA_BUZZER);
                delay(DELAY_BUZZER);
                noTone(BUZZER);
            }

            if (tentativas_restantes == 0) {
                finalizar_jogo_como_derrota();
                break;
            }

            verificar_se_palavra_foi_descoberta();

            if (vitoria) {
                finalizar_jogo_como_vitoria();
                break;
            }
        }
    }
}

void reiniciar_variaveis() {
    sortear_outra_palavra();

    ocultar_palavra_do_display(palavra_no_display);
    lcd.clear();
    lcd.print(palavra_no_display);

    noTone(BUZZER);
    vitoria = false;
    tentativas_restantes = ceil((double)(quantidade_de_letras) / 3);
    gameloop = 1;
    estado_anterior_do_botao_dentro_do_loop = 0;
}

void sortear_outra_palavra() {
    indice_do_sorteio++;

    if (indice_do_sorteio == QUANTIDADE_DE_PALAVRAS) {
        indice_do_sorteio = 0;
    }

    palavra_sorteada = PALAVRAS[indice_do_sorteio];
    quantidade_de_letras = palavra_sorteada.length();
}

void rodar_display() {
  lcd.clear(); 
  lcd.print("Vai rodando");
  delay(200);
  lcd.clear();
}