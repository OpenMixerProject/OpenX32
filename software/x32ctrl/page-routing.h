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

        void OnInit() override
		{
        	BindEncoder(DISPLAY_ENCODER_1, PAGE_CUSTOM_ENCODER);
            BindEncoder(DISPLAY_ENCODER_2, PAGE_CUSTOM_ENCODER);
		}

		void OnShow() override
        {
            custom_encoder[DISPLAY_ENCODER_1].label = "XLR In -> Channel 1-32";
			custom_encoder[DISPLAY_ENCODER_2].label = "CARD -> Channel 1-32";
		}

        bool OnDisplayButton(X32_BTN button, bool pressed) override
		{
			bool handled = true;

            if (pressed)
            {
                switch (button)
                {                
                    case X32_BTN_ENCODER1:
                        {
                            for (uint i = 0; i < 32; i++)
                            {
                                config->Set(ROUTING_FPGA, FPGA_INPUT_IDX_XLR + i, FPGA_OUTPUT_IDX_DSP - 1 + i);
                            }                    
                        }
                        break;
                    case X32_BTN_ENCODER2:
                        {
                            for (uint i = 0; i < 32; i++)
                            {
                                config->Set(ROUTING_FPGA, FPGA_INPUT_IDX_CARD + i, FPGA_OUTPUT_IDX_DSP - 1 + i);
                            }                    
                        }
                        break;
                    default:  
                        handled = false;              
                        break;
                }
            }

			return handled;
        }
};