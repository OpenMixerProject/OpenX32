#pragma once

#include "enum.h"
#include "WString.h"

using namespace std;

class SurfaceElement
{
    private:

        SurfaceElementId element_id;
        String _name;
        
        /// @brief Button-ID, LED-ID or Fader-Index
        uint element_index; 

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

        /// @brief Button-ID, LED-ID or Fader-Index
        uint GetIndex()
        {
            return element_index;
        }



};