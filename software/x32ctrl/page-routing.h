#pragma once

#include "page.h"

using namespace std;

class PageRouting: public Page {
    public:
        PageRouting(PageBaseParameter* pagebasepar) : Page(pagebasepar) {
            prevPage = X32_PAGE::NONE;
            nextPage = X32_PAGE::ROUTING_FPGA;
            tabLayer0 = objects.maintab;
            tabIndex0 = 2;
            tabLayer1 = objects.routingtab;
            tabIndex1 = 0;
            led = X32_BTN_ROUTING;
        }

        void OnShow() override {
			encoderSliders[0].label = "Default Routing";
			encoderSliders[1].label = "XLR -> Channels";
			encoderSliders[2].label = "Card -> Channels";
		}

        void OnDisplayButton(X32_BTN button, bool pressed) override {
            if (pressed){
				switch (button){
					case X32_BTN_ENCODER1:
						mixer->dsp->LoadRouting_X32Default();
						break;
					case X32_BTN_ENCODER2:
					    mixer->fpga->RoutingXlrAs32CHInput();                    
						break;
					case X32_BTN_ENCODER3:
						mixer->fpga->RoutingCardAs32CHInput();
						break;
					case X32_BTN_ENCODER4:
						
						break;
					case X32_BTN_ENCODER5:
						
						break;
					case X32_BTN_ENCODER6:
						
						break;
					default:
                        // just here to avoid compiler warnings
						break;
				}
            }
        }
};