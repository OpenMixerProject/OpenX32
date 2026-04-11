#pragma once

#include "enum.h"
#include "WString.h"

using namespace std;

class SurfaceElement
{
    private:

        SE_ID element_id;
        String _name;

    public:

        SE_TYPE element_type;
        X32_BOARD board;
        uint index;

        X32_BTN button;

        SurfaceElement(SE_ID se, String name)
        {
            element_id = se;
            _name = name;
        }

        SurfaceElement* DefFader(X32_BOARD board, uint faderindex)
        {
            element_type = SE_TYPE::Fader;
            this->board = board;
            index = faderindex;

            return this;
        }

        SurfaceElement* DefButton(X32_BTN button_id)
        {
            element_type = SE_TYPE::Button;
            button = button_id;

            return this;
        }

};