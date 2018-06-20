#ifndef PROTOCOL_TYPES_H
#define PROTOCOL_TYPES_H

namespace protocol
{

/**
 * @enum ButtonState
 * @brief Значения состояний клавиш
 */
enum class ButtonState
{
    Off = 0, //!< Клавиша разомкнута
    On  = 1  //!< Клавиша замкнута
};

/**
 * @enum ImageSelection
 * @brief Выбранные изображения
 */
enum class ImageSelection
{
    Nothing  = 0, //!< Не выбрано ни одно из изображений
    First    = 1, //!< Выбрано только первое изображение
    Second   = 2, //!< Выбрано только второе изображение
    Both     = 3, //!< Выбраны оба изображения
    End
};

/**
 * @enum BlinkState
 * @brief Параметры мигания
 */
enum class BlinkState
{
    Off = 0, //!< Мигание выключено
    On  = 1, //!< Мигание включено
    End
};

/**
 * @enum BrightLevelBound
 * @brief Граничные значения уровня яркости
 */
enum class BrightLevel
{
    Min = 0,  //!< Минимальное значение уровеня яркости
    Max = 15, //!< Максимальное значение уровеня яркости
    End
};

} // protocol

#endif // PROTOCOL_TYPES_H
