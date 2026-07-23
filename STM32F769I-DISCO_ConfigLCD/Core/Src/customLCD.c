/*
 * customLCD.c
 *
 *  Created on: Feb 6, 2026
 *      Author: Debasish Das
 */

#include "customLCD.h"
#include "custom.h"
#include "stm32f769i_discovery_ts.h"


void setLCDBox(uint32_t XPos, uint32_t YPos, uint32_t XLength, uint32_t YHeight,
		uint32_t borColor, uint32_t fillColor, uint32_t textColor,
		sFONT *pTextFont, LCDBox_t *pLcdBox)
{
	pLcdBox->XPos = XPos;
	pLcdBox->YPos = YPos;
	pLcdBox->XLength = XLength;
	pLcdBox->YHeight = YHeight;
	pLcdBox->borColor = borColor;
	pLcdBox->fillColor = fillColor;
	pLcdBox->textColor = textColor;
	pLcdBox->pTextFont = pTextFont;
}

void drawLCDBox(LCDBox_t *pLcdBox, uint8_t *pMsg, uint8_t bMsgLen)
{

	BSP_LCD_SetTextColor(pLcdBox->borColor);
	BSP_LCD_SetBackColor(pLcdBox->borColor);
	BSP_LCD_SetTextColor(pLcdBox->textColor);

	BSP_LCD_FillRect(
			pLcdBox->XPos,
			pLcdBox->YPos,
			pLcdBox->XLength,
			pLcdBox->YHeight);

	BSP_LCD_SetFont(pLcdBox->pTextFont);

	BSP_LCD_DisplayStringAt(
			pLcdBox->XPos+10,
			pLcdBox->YPos+(pLcdBox->YHeight)/2,
			pMsg,
			LEFT_MODE
			);
}

uint32_t getColorRGB(uint8_t bColor)
{
	switch (bColor)
		{
	case COLOR_BLUE:
		return LCD_COLOR_BLUE;
	case COLOR_GREEN:
		return LCD_COLOR_GREEN;
	case COLOR_RED:
		return LCD_COLOR_RED;
	case COLOR_CYAN:
		return LCD_COLOR_CYAN;
	case COLOR_MAGENTA:
		return LCD_COLOR_MAGENTA;
	case COLOR_YELLOW:
		return LCD_COLOR_YELLOW;
	case COLOR_LIGHTBLUE:
		return LCD_COLOR_LIGHTBLUE;
	case COLOR_LIGHTGREEN:
		return LCD_COLOR_LIGHTGREEN;
	case COLOR_LIGHTRED:
		return LCD_COLOR_LIGHTRED;
	case COLOR_LIGHTCYAN:
		return LCD_COLOR_LIGHTCYAN;
	case COLOR_LIGHTMAGENTA:
		return LCD_COLOR_LIGHTMAGENTA;
	case COLOR_LIGHTYELLOW:
		return LCD_COLOR_LIGHTYELLOW;
	case COLOR_DARKBLUE:
		return LCD_COLOR_DARKBLUE;
	case COLOR_DARKGREEN:
		return LCD_COLOR_DARKGREEN;
	case COLOR_DARKRED:
		return LCD_COLOR_DARKRED;
	case COLOR_DARKCYAN:
		return LCD_COLOR_DARKCYAN;
	case COLOR_DARKMAGENTA:
		return LCD_COLOR_DARKMAGENTA;
	case COLOR_DARKYELLOW:
		return LCD_COLOR_DARKYELLOW;
	case COLOR_WHITE:
		return LCD_COLOR_WHITE;
	case COLOR_LIGHTGRAY:
		return LCD_COLOR_LIGHTGRAY;
	case COLOR_GRAY:
		return LCD_COLOR_GRAY;
	case COLOR_DARKGRAY:
		return LCD_COLOR_DARKGRAY;
	case COLOR_BLACK:
		return LCD_COLOR_BLACK;
	case COLOR_BROWN:
		return LCD_COLOR_BROWN;
	case COLOR_ORANGE:
		return LCD_COLOR_ORANGE;
	case COLOR_TRANSPARENT:
		return LCD_COLOR_TRANSPARENT;

	default:
		return ERROR;
		}
}

void drawCircleMultiColors(uint8_t bColor)
{
	BSP_LCD_SetTextColor(getColorRGB(bColor));
	BSP_LCD_FillCircle(400, 360, 50);
}

void EXTI15_10_IRQHandler(void)
{
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_13);
}
extern volatile uint8_t TOUCH_ON;
extern volatile uint16_t pixelXY[2];
extern TS_StateTypeDef TS_State;
extern uint8_t touchCount;


void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	if (GPIO_Pin == GPIO_PIN_13)
	{
		TOUCH_ON = 1;
		touchCount++;
	}
}



void MPU_SDRAM_Config()
	{
		MPU_Region_InitTypeDef MPU_InitStruct = { 0 };

		HAL_MPU_Disable();

		/* SDRAM: 0xC0000000, 8MB */
		MPU_InitStruct.Enable = MPU_REGION_ENABLE;
		MPU_InitStruct.BaseAddress = 0xC0000000;
		MPU_InitStruct.Size = MPU_REGION_SIZE_8MB;
		MPU_InitStruct.AccessPermission = MPU_REGION_FULL_ACCESS;
		MPU_InitStruct.IsBufferable = MPU_ACCESS_NOT_BUFFERABLE;
		MPU_InitStruct.IsCacheable = MPU_ACCESS_CACHEABLE;
		MPU_InitStruct.IsShareable = MPU_ACCESS_NOT_SHAREABLE;
		MPU_InitStruct.Number = MPU_REGION_NUMBER0;
		MPU_InitStruct.TypeExtField = MPU_TEX_LEVEL1;
		MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_ENABLE;

		HAL_MPU_ConfigRegion(&MPU_InitStruct);

		HAL_MPU_Enable(MPU_PRIVILEGED_DEFAULT);
	}
