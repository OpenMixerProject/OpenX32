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
            config->GetParameter(DISPLAY_ENCODER_1_BUTTON)->SetName("XLR 1 - 32\ninto\nChannel 1 - 32");
            config->GetParameter(DISPLAY_ENCODER_2_BUTTON)->SetName("CARD 1 - 32\ninto\nChannel 1 - 32");
            config->GetParameter(DISPLAY_ENCODER_3_BUTTON)->SetName("XLR 1 - 32\ninto\nAES50 1 - 32");
            config->GetParameter(DISPLAY_ENCODER_4_BUTTON)->SetName("CARD 1 - 32\ninto\nAES50 1 - 32");
		}

        void OnChange(bool force) override
		{
			if (config->HasParameterChanged(DISPLAY_ENCODER_1_BUTTON))
            {
                for (uint i = 0; i < 32; i++)
                {
                    config->Set(ROUTING_FPGA, FPGA_INPUT_IDX_XLR + i, FPGA_OUTPUT_IDX_DSP - 1 + i);
                }                    
            }

            if (config->HasParameterChanged(DISPLAY_ENCODER_2_BUTTON))
            {
                for (uint i = 0; i < 32; i++)
                {
                    config->Set(ROUTING_FPGA, FPGA_INPUT_IDX_CARD + i, FPGA_OUTPUT_IDX_DSP - 1 + i);
                }                    
            }

            if (config->HasParameterChanged(DISPLAY_ENCODER_3_BUTTON))
            {
                for (uint i = 0; i < 32; i++)
                {
                    config->Set(ROUTING_FPGA, FPGA_INPUT_IDX_XLR + i, FPGA_OUTPUT_IDX_AES50A - 1 + i);
                }                    
            }

            if (config->HasParameterChanged(DISPLAY_ENCODER_4_BUTTON))
            {
                for (uint i = 0; i < 32; i++)
                {
                    config->Set(ROUTING_FPGA, FPGA_INPUT_IDX_CARD + i, FPGA_OUTPUT_IDX_AES50A - 1 + i);
                }                    
            }
        }
};