/**
 * @file    drv_gyroscope.c
 * @author  Jeremie Gallee (jeremie.gallee@noser.com)
 * @brief   Driver for gyroscope LPR5150AL (obsolete)
 *          Also offers a read to inner temperature through ADC
 * @version 0.1
 * @date    2025-09-10
 * 
 * @copyright Copyright (c) 2025
 * 
 */

/**
 * @brief   Measure values of ADC to send random data to MQTT broker
 * @param   none
 * @return  [float] value of ADC
 */
float drv_read_temperature(void)
{
   /* 12-bit conversion, assume max value == ADC_VREF == 3.3 V */
   const float conversionFactor = 3.3f / (1 << 12);

   adc_select_input(ADC_PIN_ITEMP);
   float temper_adc = (float)adc_read() * conversionFactor;
   float temper_value = 27.0f - (temper_adc - 0.706f) / 0.001721f;

   return temper_value;
}

/**
 * @brief read ADCs and convert it to angular speed
 * LPR5150AL Sensitivity: 0.67f [mV/dps] in 4x mode
 * @param gyrValues pointer to structure to fill with angular values
 */
void drv_read_gyroscope(gyros_vals* gyrValues)
{
   adc_select_input(ADC_PIN_XAXIS);
   gyrValues->xaxis = (uint16_t)adc_read();

   adc_select_input(ADC_PIN_YAXIS);
   gyrValues->yaxis = (uint16_t)adc_read();
}
