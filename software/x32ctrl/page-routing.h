#pragma once

#include "page.h"

using namespace std;

class PageRouting: public Page
{
    public:

        PageRouting(PageBaseParameter* pagebasepar) : Page(pagebasepar)
		{
            prevPage = X32_PAGE::NONE;
            nextPage = X32_PAGE::ROUTING_FPGA;
            tabLayer0 = objects.maintab;
            tabIndex0 = 2;
            tabLayer1 = objects.routingtab;
            tabIndex1 = 0;
            led = X32_BTN_ROUTING;
        }

		void OnShow() override
        {
            config->SurfaceBindCustom(SurfaceElementId::DISPLAY_ENCODER_1, "Preset:\nXLR 1 - 32 to\nChannel 1 - 32");
            config->SurfaceBindCustom(SurfaceElementId::DISPLAY_ENCODER_BUTTON_1);
            config->SurfaceBindCustom(SurfaceElementId::DISPLAY_ENCODER_2, "Preset:\nCARD 1 - 32 to\nChannel 1 - 32");
            config->SurfaceBindCustom(SurfaceElementId::DISPLAY_ENCODER_BUTTON_2);
            config->SurfaceBindCustom(SurfaceElementId::DISPLAY_ENCODER_3, "Preset:\nXLR 1 - 32 to\nAES50 1 - 32");
            config->SurfaceBindCustom(SurfaceElementId::DISPLAY_ENCODER_BUTTON_3);
            config->SurfaceBindCustom(SurfaceElementId::DISPLAY_ENCODER_4, "Preset:\nCARD 1 - 32 to\nAES50 1 - 32");
            config->SurfaceBindCustom(SurfaceElementId::DISPLAY_ENCODER_BUTTON_4);
		}

        void OnChangeCustomButton(SurfaceElementId surface_element_id) override
        {
            switch (surface_element_id)
            {
                case SurfaceElementId::DISPLAY_ENCODER_BUTTON_1:
                    for (uint i = 0; i < 32; i++)
                    {
                        config->Set(ROUTING_FPGA, FPGA_INPUT_IDX_XLR + i, FPGA_OUTPUT_IDX_DSP - 1 + i);
                    }
                    break;
                 case SurfaceElementId::DISPLAY_ENCODER_BUTTON_2:
                    for (uint i = 0; i < 32; i++)
                    {
                        config->Set(ROUTING_FPGA, FPGA_INPUT_IDX_CARD + i, FPGA_OUTPUT_IDX_DSP - 1 + i);
                    }  
                    break;
                 case SurfaceElementId::DISPLAY_ENCODER_BUTTON_3:
                    for (uint i = 0; i < 32; i++)
                    {
                        config->Set(ROUTING_FPGA, FPGA_INPUT_IDX_XLR + i, FPGA_OUTPUT_IDX_AES50A - 1 + i);
                    }   
                    break;
                 case SurfaceElementId::DISPLAY_ENCODER_BUTTON_4:
                    for (uint i = 0; i < 32; i++)
                    {
                        config->Set(ROUTING_FPGA, FPGA_INPUT_IDX_CARD + i, FPGA_OUTPUT_IDX_AES50A - 1 + i);
                    }  
                    break;
                default:
                    break;
            }
        }
};