#include <stdio.h> /
#include <stdlib.h>
#include <freertos/FreeRTOS.h>
#include "i2c-lcd.h"
#include "esp_log.h"
#include "driver/i2c.h"

static const char *tag = "Main";

#define TRUE 1
#define FALSE 0

#define ESTADO_INIT         0
#define ESTADO_ABRIENDO     1
#define ESTADO_CERRANDO     2
#define ESTADO_CERRADO      3
#define ESTADO_ABIERTO      4
#define ESTADO_EMERGENCIA   5
#define ESTADO_ERROR        6
#define ESTADO_ESPERA       7

TimerHandle_t xTimers;
int timerID = 1;
int INTERVALO = 50;

esp_err_t INTERRUPCION_50MS(void);
esp_err_t SET_TIMER(void);
static esp_err_t i2c_master_init(void);

static const char *TAG = "i2c-simple-example";

int Func_ESTADO_INIT();
int Func_ESTADO_ABRIENDO();
int Func_ESTADO_CERRANDO();
int Func_ESTADO_CERRADO();
int Func_ESTADO_ABIERTO();
int Func_ESTADO_EMERGENCIA();
int Func_ESTADO_ERROR();
int Func_ESTADO_ESPERA();

#define TRUE 1
#define FALSE 0

#define ESTADO_INIT         0
#define ESTADO_ABRIENDO     1
#define ESTADO_CERRANDO     2
#define ESTADO_CERRADO      3
#define ESTADO_ABIERTO      4
#define ESTADO_EMERGENCIA   5
#define ESTADO_ERROR        6
#define ESTADO_ESPERA       7



volatile int ESTADO_ACTUAL = ESTADO_INIT;
volatile int ESTADO_SIGUIENTE = ESTADO_INIT;
volatile int ESTADO_ANTERIOR = ESTADO_INIT;
volatile unsigned int TimeCa = 0;
volatile int CONTADOR = 0;


volatile struct INPUTS{
    unsigned int LSA: 1;
    unsigned int LSC: 1;
    unsigned int CA:  1;
    unsigned int CC:  1;
    unsigned int FC:  1;
}inputs;
volatile struct OUTPUTS{
    unsigned int MC: 1;
    unsigned int MA: 1;
    unsigned int LED_EMERGENCIA: 1;
    unsigned int LED_MOVIMIENTO: 1;
}outputs;


/**************************************************** */

void vTimerCallback( TimerHandle_t pxTimer){

    if(ESTADO_ACTUAL == ESTADO_CERRANDO || ESTADO_ACTUAL == ESTADO_ABRIENDO){
        if(inputs.CA = TRUE || inputs.CA == TRUE){

            CONTADOR = 0;
        }
        CONTADOR++;

    }else{

        CONTADOR = 0;
    }
    INTERRUPCION_50MS();
}

void app_main() {


    ESP_ERROR_CHECK(i2c_master_init());
    ESP_LOGI(TAG, "I2C initialized successfully");

    ESTADO_SIGUIENTE = Func_ESTADO_INIT();
    SET_TIMER();

    lcd_put_cur(0,0);
    lcd_send_string("Albert Cabra");
    lcd_put_cur(1,0);
    lcd_send_string("PUERTON.");
    lcd_clear();



    for (;;)
    {
        if (ESTADO_SIGUIENTE == ESTADO_INIT)
        {
            ESTADO_SIGUIENTE = Func_ESTADO_INIT();
        }
        if (ESTADO_SIGUIENTE == ESTADO_ESPERA)
        {
            ESTADO_SIGUIENTE = Func_ESTADO_ESPERA();
        }
        if (ESTADO_SIGUIENTE == ESTADO_ABRIENDO)
        {
            ESTADO_SIGUIENTE = Func_ESTADO_ABRIENDO();
        }
        if (ESTADO_SIGUIENTE == ESTADO_CERRANDO)
        {
            ESTADO_SIGUIENTE = Func_ESTADO_CERRANDO();
        }
        if (ESTADO_SIGUIENTE == ESTADO_CERRADO)
        {
            ESTADO_SIGUIENTE = Func_ESTADO_CERRADO();
        }
        if (ESTADO_SIGUIENTE == ESTADO_ABIERTO)
        {
            ESTADO_SIGUIENTE = Func_ESTADO_ABIERTO();
        }
        if (ESTADO_SIGUIENTE == ESTADO_EMERGENCIA)
        {
            ESTADO_SIGUIENTE = Func_ESTADO_EMERGENCIA();
        }
        if (ESTADO_SIGUIENTE == ESTADO_ERROR)
        {
            ESTADO_SIGUIENTE = Func_ESTADO_ERROR();
        }

    }

}

//TERMINADA
int Func_ESTADO_INIT(){

    ESP_LOGE(tag, "INICIANDO PROGRAMA");
    lcd_put_cur(0,0);
    lcd_send_string("INICIANDO");


    ESTADO_ANTERIOR = ESTADO_ACTUAL;
    ESTADO_ACTUAL = ESTADO_INIT;

    ESP_LOGE(tag, "CONFIGURACION DE LOS PINES");

    gpio_config_t IO_CONFIG;
    IO_CONFIG.mode = GPIO_MODE_INPUT;
    IO_CONFIG.pin_bit_mask = (1 << 13) | (1 << 12) | (1 << 14) | (1 << 27) | (1 << 26);
    IO_CONFIG.pull_down_en = GPIO_PULLDOWN_ENABLE;
    IO_CONFIG.pull_up_en = GPIO_PULLUP_DISABLE;
    IO_CONFIG.intr_type = GPIO_INTR_DISABLE;
    gpio_config(&IO_CONFIG);

    //CONFIGURANDO LAS SALIDAS
    IO_CONFIG.mode = GPIO_MODE_OUTPUT;
    IO_CONFIG.pin_bit_mask = (1 << 4) | (1 << 16) | (1 << 17) | (1 << 5);
    IO_CONFIG.pull_down_en = GPIO_PULLDOWN_DISABLE;
    IO_CONFIG.pull_up_en = GPIO_PULLUP_DISABLE;
    IO_CONFIG.intr_type = GPIO_INTR_DISABLE;
    gpio_config(&IO_CONFIG);

    for (;;)
    {
        return ESTADO_ESPERA;
    }
}

int Func_ESTADO_ABRIENDO(){

    printf("ABRIENDO EL PUERTON");
    lcd_clear();
    lcd_put_cur(0,0);
    lcd_send_string("ABRIENDO");
    lcd_put_cur(1,0);
    lcd_send_string("PUERTON");

    ESTADO_ANTERIOR = ESTADO_ACTUAL;
    ESTADO_ACTUAL = ESTADO_ABRIENDO;

    outputs.LED_MOVIMIENTO = TRUE;
    outputs.LED_EMERGENCIA = FALSE;
    outputs.MA = TRUE;
    outputs.MC = FALSE;

    for (;;)
    {
        if(inputs.LSA == TRUE){

            return ESTADO_ABIERTO;
        }
        if (inputs.LSA == TRUE && inputs.LSC == TRUE)
        {
            return ESTADO_ERROR;
        }
        if (inputs.FC == TRUE)
        {
            return ESTADO_EMERGENCIA;
        }
        if(inputs.CC == TRUE ){

            return ESTADO_CERRANDO;
        }

        if (CONTADOR == 3600)
        {
            return ESTADO_ERROR;
        }

    }
}


int Func_ESTADO_CERRANDO(){

    printf("CERRANDO EL PUERTON");
    lcd_clear();
    lcd_put_cur(0,0);
    lcd_send_string("CERRANDO");
    lcd_put_cur(1,0);
    lcd_send_string("PUERTON");

    ESTADO_ANTERIOR = ESTADO_ACTUAL;
    ESTADO_ACTUAL = ESTADO_CERRANDO;

    outputs.LED_MOVIMIENTO = TRUE;
    outputs.LED_EMERGENCIA = FALSE;
    outputs.MA = FALSE;
    outputs.MC = TRUE;

    for (;;)
    {
        if (inputs.LSC == TRUE )
        {
            return ESTADO_CERRADO;
        }

        if (inputs.LSA == TRUE && inputs.LSC == TRUE)
        {
            return ESTADO_ERROR;
        }

        if (inputs.FC == TRUE)
        {
            return ESTADO_EMERGENCIA;
        }
        if (inputs.CA == TRUE)
        {
            return ESTADO_ABRIENDO;
        }
        if (CONTADOR == 3600)
        {
            return ESTADO_ERROR;
        }

    }
}


int Func_ESTADO_CERRADO(){

    printf("PUERTON CERRADO ");
    ESTADO_ANTERIOR = ESTADO_ACTUAL;
    ESTADO_ACTUAL = ESTADO_CERRADO;
    lcd_clear();
    lcd_put_cur(0,0);
    lcd_send_string("PUERTON");
    lcd_put_cur(1,0);
    lcd_send_string("CERRADO");

    outputs.LED_MOVIMIENTO = FALSE;
    outputs.LED_EMERGENCIA = FALSE;
    outputs.MA = FALSE;
    outputs.MC = FALSE;

    for (;;)
    {
        return ESTADO_ESPERA;
    }

}


int Func_ESTADO_ABIERTO(){

    printf("PUERTON ABIERTO ");
    lcd_clear();
    lcd_put_cur(0,0);
    lcd_send_string("PUERTON");
    lcd_put_cur(1,0);
    lcd_send_string("ABIERTO");
    ESTADO_ANTERIOR = ESTADO_ACTUAL;
    ESTADO_ACTUAL = ESTADO_ABIERTO;

    outputs.LED_MOVIMIENTO = FALSE;
    outputs.LED_EMERGENCIA = FALSE;
    outputs.MA = FALSE;
    outputs.MC = FALSE;

    for (;;)
    {
        return ESTADO_ESPERA;
    }
}


int Func_ESTADO_EMERGENCIA(){

    printf("EMERGENCIA!!!!!!!");
    lcd_clear();
    lcd_put_cur(0,0);
    lcd_send_string("EMERGENCIA!!!");

    ESTADO_ANTERIOR = ESTADO_ACTUAL;
    ESTADO_ACTUAL = ESTADO_EMERGENCIA;

    outputs.LED_MOVIMIENTO = FALSE;
    outputs.LED_EMERGENCIA = TRUE;
    outputs.MA = FALSE;
    outputs.MC = FALSE;



    for (;;)
    {
        vTaskDelay(1500/portTICK_PERIOD_MS);
        if (inputs.FC == FALSE)
        {
            return ESTADO_ANTERIOR;
        }

    }
}


int Func_ESTADO_ERROR(){

    lcd_clear();
    lcd_put_cur(0,3);
    lcd_send_string("ERROR!!!");
    lcd_put_cur(1,3);
    lcd_send_string("ERROR!!!");

    ESTADO_ANTERIOR = ESTADO_ACTUAL;
    ESTADO_ACTUAL = ESTADO_ERROR;

    outputs.LED_MOVIMIENTO = FALSE;
    outputs.LED_EMERGENCIA = TRUE;
    outputs.MA = FALSE;
    outputs.MC = FALSE;

    printf("\nERROR!!!!");
    printf("\nERROR!!!!");
    printf("\nERROR!!!!");

    for (;;)
    {
        vTaskDelay(500/portTICK_PERIOD_MS);
        outputs.LED_EMERGENCIA = FALSE;
        vTaskDelay(500/portTICK_PERIOD_MS);
        outputs.LED_EMERGENCIA = TRUE;
        vTaskDelay(500/portTICK_PERIOD_MS);
        outputs.LED_EMERGENCIA = FALSE;

        return ESTADO_ESPERA;

    }
}


int Func_ESTADO_ESPERA(){

    lcd_clear();
    lcd_put_cur(0,0);
    lcd_send_string("ESPERANDO");
    //Actualizar Variables de estado
    ESTADO_ANTERIOR = ESTADO_ACTUAL;
    ESTADO_ACTUAL = ESTADO_ESPERA;

    outputs.LED_EMERGENCIA = FALSE;
    outputs.LED_MOVIMIENTO = FALSE;
    outputs.MA = FALSE;
    outputs.MC = FALSE;
    ESP_LOGE(tag, "ESTADO ESPERA");

    for (;;)
    {

        if (inputs.LSA == FALSE && inputs.FC == FALSE && inputs.LSC == FALSE)//Si se encuentra en un estado entre abierto
                                                                             //Por falla en el sistema o por falta de luz
        {
            return ESTADO_CERRANDO;
        }
        if (inputs.CA == TRUE && inputs.FC == FALSE && inputs.LSA == FALSE && inputs.FC == FALSE)
        {
            return ESTADO_ABRIENDO;
        }
        if (inputs.CC == TRUE && inputs.FC == FALSE)
        {
            return ESTADO_CERRANDO;
        }
        if (inputs.CA == TRUE && inputs.FC == FALSE)
        {
            return ESTADO_ABRIENDO;
        }

        if (inputs.FC == TRUE)
        {
            return ESTADO_EMERGENCIA;
        }
        if(inputs.LSA == TRUE && inputs.LSC == TRUE)
        {
            return ESTADO_ERROR;
        }

    }
}


esp_err_t SET_TIMER(void){

    printf("Inicializando configuracion del timer...");

    xTimers = xTimerCreate("Timer",
                          (pdMS_TO_TICKS(INTERVALO)),
                          pdTRUE,
                          ( void * )timerID,
                          vTimerCallback
);

        if (xTimers == NULL)
        {
            printf("El timer no fue creado");
        }else
        {
            if (xTimerStart(xTimers, 0) != pdPASS)
            {
                printf("El timer podria no ser seteado en el estado activo");
            }

        }

    return ESP_OK;
}

esp_err_t INTERRUPCION_50MS(void){

    inputs.LSC = (gpio_get_level(13) == TRUE)?TRUE:FALSE;
    inputs.LSA = (gpio_get_level(12) == TRUE)?TRUE:FALSE;
    inputs.FC = (gpio_get_level(14) == TRUE)?TRUE:FALSE;
    inputs.CC = (gpio_get_level(27) == TRUE)?TRUE:FALSE;
    inputs.CA = (gpio_get_level(26) == TRUE)?TRUE:FALSE;

    if (outputs.LED_MOVIMIENTO == TRUE)
    {
        gpio_set_level(4,TRUE);
    }else
    {
        gpio_set_level(4,FALSE);
    }

    if (outputs.LED_EMERGENCIA == TRUE)
    {
        gpio_set_level(16,TRUE);
    }else
    {
        gpio_set_level(16,FALSE);
    }
    if (outputs.MC == TRUE)
    {
        gpio_set_level(17,TRUE);
    }else
    {
        gpio_set_level(17,FALSE);
    }
    if (outputs.MA == TRUE)
    {
        gpio_set_level(5,TRUE);
    }else
    {
        gpio_set_level(5,FALSE);
    }


    return ESP_OK;
}


static esp_err_t i2c_master_init(void){

    int i2c_master_port = I2C_NUM_0;

    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = GPIO_NUM_21,
        .scl_io_num = GPIO_NUM_22,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = 100000,
    };

    i2c_param_config(i2c_master_port, &conf);

    return i2c_driver_install(i2c_master_port, conf.mode, 0, 0, 0);
}

