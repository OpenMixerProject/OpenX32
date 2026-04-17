#pragma once

#include "external.h"
#include "enum.h"
#include "WString.h"

using namespace std;

class SurfaceElement
{
    private:

        SurfaceElementId element_id;
        String _name;
        
        // Button-ID, LED-ID or Fader-Index
        uint element_index;

        uint encoder_backlight;
        uint encoder_button;

        bool no_led = false;

        X32_BOARD board_id;

    public:

        SurfaceElementType element_type;


        SurfaceElement(SurfaceElementId se, String name)
        {
            element_id = se;
            _name = name;
        }

        SurfaceElement* DefFader(X32_BOARD board, uint faderindex)
        {
            element_type = SurfaceElementType::Fader;
            board_id = board;
            element_index = faderindex;

            return this;
        }

        SurfaceElement* DefButton(X32_BOARD board, uint button)
        {
            element_type = SurfaceElementType::Button;
            board_id = board;
            element_index = button;

            return this;
        }

        SurfaceElement* DefLcd(X32_BOARD board, uint lcd)
        {
            element_type = SurfaceElementType::Lcd;
            board_id = board;
            element_index = lcd;

            return this;
        }

        SurfaceElement* DefLed(X32_BOARD board, uint led)
        {
            element_type = SurfaceElementType::Led;
            board_id = board;
            element_index = led;

            return this;
        }

        SurfaceElement* DefEncoder(X32_BOARD board, uint encoder, uint backlight = 0)
        {
            element_type = SurfaceElementType::Encoder;
            board_id = board;
            element_index = encoder;
            encoder_backlight = backlight;

            return this;
        }

        SurfaceElement* DefPushEncoder(X32_BOARD board, uint encoder, uint button)
        {
            element_type = SurfaceElementType::PushEncoder;
            board_id = board;
            element_index = encoder;
            encoder_button = button;

            return this;
        }

        SurfaceElement* DefNoLed()
        {
            // if (element_type != SurfaceElementType::Button)
            // {
            //     throw new Exception();
            // }
            
            no_led = true;

            return this;
        }

        String GetName()
        {
            return _name;
        }

        SurfaceElementId GetId()
        {
            return element_id;
        }

        X32_BOARD GetBoard()
        {
            return board_id;
        }

        // Button-ID, LED-ID or Fader-Index
        uint GetIndex()
        {
            return element_index;
        }


        uint GetEncoderBacklight()
        {
            if (element_type == SurfaceElementType::Encoder)
            {
                return encoder_backlight;
            }

            return 0;
        }

        uint GetPushEncoderButton()
        {
            if (element_type == SurfaceElementType::PushEncoder)
            {
                return encoder_button;
            }

            return 0;
        }


};