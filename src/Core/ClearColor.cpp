#include <SDL.h>
#include "PatataEngine/PatataEngine.hpp"

void
Patata::Engine::ClearColorRGBA (const float & R, const float & G,
                                const float & B, const float & A)
{
  pClearColor->r = R;
  pClearColor->g = G;
  pClearColor->b = B;
  pClearColor->a = A;
}

void
Patata::Engine::ClearColorRGBA (const float & R, const float & G,
                               const float & B)
{
  pClearColor->r = R;
  pClearColor->g = G;
  pClearColor->b = B;
}

void
Patata::Engine::ClearColor (const eColor & Color)
{
  switch (Color)
    {
    // Basic Colors
    case eColor::Black:
      pClearColor->r = 0.0f;
      pClearColor->g = 0.0f;
      pClearColor->b = 0.0f;
      break;

    case eColor::Silver:
      pClearColor->r = 0.7529411764705882f;
      pClearColor->g = 0.7529411764705882f;
      pClearColor->b = 0.7529411764705882f;
      break;

    case eColor::Gray:
      pClearColor->r = 0.5019607843137255f;
      pClearColor->g = 0.5019607843137255f;
      pClearColor->b = 0.5019607843137255f;
      break;

    case eColor::White:
      pClearColor->r = 1.0f;
      pClearColor->g = 1.0f;
      pClearColor->b = 1.0f;
      break;

    case eColor::Maroon:
      pClearColor->r = 0.5019607843137255f;
      pClearColor->g = 0.0f;
      pClearColor->b = 0.0f;
      break;

    case eColor::Red:
      pClearColor->r = 1.0f;
      pClearColor->g = 0.0f;
      pClearColor->b = 0.0f;
      break;

    case eColor::Purple:
      pClearColor->r = 0.5019607843137255f;
      pClearColor->g = 0.0f;
      pClearColor->b = 0.5019607843137255f;
      break;

    case eColor::Fuchsia:
      pClearColor->r = 1.0f;
      pClearColor->g = 0.0f;
      pClearColor->b = 1.0f;
      break;

    case eColor::Green:
      pClearColor->r = 0.0f;
      pClearColor->g = 0.5019607843137255f;
      pClearColor->b = 0.0f;
      break;

    case eColor::Lime:
      pClearColor->r = 0.0f;
      pClearColor->g = 1.0f;
      pClearColor->b = 0.0f;
      break;

    case eColor::Olive:
      pClearColor->r = 0.5019607843137255f;
      pClearColor->g = 0.5019607843137255f;
      pClearColor->b = 0.0f;
      break;

    case eColor::Yellow:
      pClearColor->r = 1.0f;
      pClearColor->g = 1.0f;
      pClearColor->b = 0.0f;
      break;

    case eColor::Navy:
      pClearColor->r = 0.0f;
      pClearColor->g = 0.0f;
      pClearColor->b = 0.5019607843137255f;
      break;

    case eColor::Blue:
      pClearColor->r = 0.0f;
      pClearColor->g = 0.0f;
      pClearColor->b = 1.0f;
      break;

    case eColor::Teal:
      pClearColor->r = 0.0f;
      pClearColor->g = 0.5019607843137255f;
      pClearColor->b = 0.5019607843137255f;
      break;

    case eColor::Aqua:
      pClearColor->r = 0.0f;
      pClearColor->g = 1.0f;
      pClearColor->b = 1.0f;
      break;

    // Extended colors
    case eColor::Aliceblue:
      pClearColor->r = 0.9411764705882353f;
      pClearColor->g = 0.9725490196078431f;
      pClearColor->b = 1.0f;
      break;

    case eColor::Antiquewhite:
      pClearColor->r = 0.9803921568627451f;
      pClearColor->g = 0.9215686274509803f;
      pClearColor->b = 0.8431372549019608f;
      break;

    case eColor::Aquamarine:
      pClearColor->r = 0.4980392156862745f;
      pClearColor->g = 1.0f;
      pClearColor->b = 0.8313725490196079f;
      break;

    case eColor::Azure:
      pClearColor->r = 0.9411764705882353f;
      pClearColor->g = 1.0f;
      pClearColor->b = 1.0f;
      break;

    case eColor::Beige:
      pClearColor->r = 0.9607843137254902f;
      pClearColor->g = 0.9607843137254902f;
      pClearColor->b = 0.8627450980392157f;
      break;

    case eColor::Bisque:
      pClearColor->r = 1.0f;
      pClearColor->g = 0.8941176470588236f;
      pClearColor->b = 0.7686274509803922f;
      break;

    case eColor::Blanchedalmond:
      pClearColor->r = 1.0f;
      pClearColor->g = 0.9215686274509803f;
      pClearColor->b = 0.803921568627451f;
      break;

    case eColor::Blueviolet:
      pClearColor->r = 0.5411764705882353f;
      pClearColor->g = 0.16862745098039217f;
      pClearColor->b = 0.8862745098039215f;
      break;

    case eColor::Brown:
      pClearColor->r = 0.6470588235294118f;
      pClearColor->g = 0.16470588235294117f;
      pClearColor->b = 0.16470588235294117f;
      break;

    case eColor::Burlywood:
      pClearColor->r = 0.8705882352941177f;
      pClearColor->g = 0.7215686274509804f;
      pClearColor->b = 0.5294117647058824f;
      break;

    case eColor::Cadetblue:
      pClearColor->r = 0.37254901960784315f;
      pClearColor->g = 0.6196078431372549f;
      pClearColor->b = 0.6274509803921569f;
      break;

    case eColor::Chartreuse:
      pClearColor->r = 0.4980392156862745f;
      pClearColor->g = 1.0f;
      pClearColor->b = 0.0f;
      break;

    case eColor::Chocolate:
      pClearColor->r = 0.8235294117647058f;
      pClearColor->g = 0.4117647058823529f;
      pClearColor->b = 0.11764705882352941f;
      break;

    case eColor::Coral:
      pClearColor->r = 1.0f;
      pClearColor->g = 0.4980392156862745f;
      pClearColor->b = 0.3137254901960784f;
      break;

    case eColor::Cornflowerblue:
      pClearColor->r = 0.39215686274509803f;
      pClearColor->g = 0.5843137254901961f;
      pClearColor->b = 0.9294117647058824f;
      break;

    case eColor::Cornsilk:
      pClearColor->r = 1.0f;
      pClearColor->g = 0.9725490196078431f;
      pClearColor->b = 0.8627450980392157f;
      break;

    case eColor::Crimson:
      pClearColor->r = 0.8627450980392157f;
      pClearColor->g = 0.0784313725490196f;
      pClearColor->b = 0.23529411764705882f;
      break;

    case eColor::Cyan:
      pClearColor->r = 0.0f;
      pClearColor->g = 1.0f;
      pClearColor->b = 1.0f;
      break;

    case eColor::Darkblue:
      pClearColor->r = 0.0f;
      pClearColor->g = 0.0f;
      pClearColor->b = 0.5450980392156862f;
      break;

    case eColor::Darkcyan:
      pClearColor->r = 0.0f;
      pClearColor->g = 0.5450980392156862f;
      pClearColor->b = 0.5450980392156862f;
      break;

    case eColor::Darkgoldenrod:
      pClearColor->r = 0.7215686274509804f;
      pClearColor->g = 0.5254901960784314f;
      pClearColor->b = 0.043137254901960784f;
      break;

    case eColor::Darkgray:
      pClearColor->r = 0.6627450980392157f;
      pClearColor->g = 0.6627450980392157f;
      pClearColor->b = 0.6627450980392157f;
      break;

    case eColor::Darkgreen:
        pClearColor->r = 0.0f;
        pClearColor->g = 0.39215686274509803f;
        pClearColor->b = 0.0f;
        break;

    case eColor::Darkkhaki:
      pClearColor->r = 0.7411764705882353f;
      pClearColor->g = 0.7176470588235294f;
      pClearColor->b = 0.4196078431372549f;
      break;

    case eColor::Darkmagenta:
      pClearColor->r = 0.5450980392156862f;
      pClearColor->g = 0.0f;
      pClearColor->b = 0.5450980392156862f;
      break;

    case eColor::Darkolivegreen:
      pClearColor->r = 0.3333333333333333f;
      pClearColor->g = 0.4196078431372549f;
      pClearColor->b = 0.1843137254901961f;
      break;

    case eColor::Darkorange:
      pClearColor->r = 1.0f;
      pClearColor->g = 0.5490196078431373f;
      pClearColor->b = 0.0f;
      break;

    case eColor::Darkorchid:
      pClearColor->r = 0.6f;
      pClearColor->g = 0.19607843137254902f;
      pClearColor->b = 0.8f;
      break;

    case eColor::Darkred:
      pClearColor->r = 0.5450980392156862f;
      pClearColor->g = 0.0f;
      pClearColor->b = 0.0f;
      break;

    case eColor::Darksalmon:
      pClearColor->r = 0.9137254901960784f;
      pClearColor->g = 0.5882352941176471f;
      pClearColor->b = 0.47843137254901963f;
      break;

    case eColor::Darkseagreen:
      pClearColor->r = 0.5607843137254902f;
      pClearColor->g = 0.7372549019607844f;
      pClearColor->b = 0.5607843137254902f;
      break;

    case eColor::Darkslateblue:
      pClearColor->r = 0.2823529411764706f;
      pClearColor->g = 0.23921568627450981f;
      pClearColor->b = 0.5450980392156862f;
      break;

    case eColor::Darkslategray:
      pClearColor->r = 0.1843137254901961f;
      pClearColor->g = 0.30980392156862746f;
      pClearColor->b = 0.30980392156862746f;
      break;

    case eColor::Darkturquoise:
      pClearColor->r = 0.0f;
      pClearColor->g = 0.807843137254902f;
      pClearColor->b = 0.8196078431372549f;
      break;

    case eColor::Darkviolet:
      pClearColor->r = 0.5803921568627451f;
      pClearColor->g = 0.0f;
      pClearColor->b = 0.8274509803921568f;
      break;

    case eColor::Deeppink:
      pClearColor->r = 1.0f;
      pClearColor->g = 0.0784313725490196f;
      pClearColor->b = 0.5764705882352941f;
      break;

    case eColor::Deepskyblue:
      pClearColor->r = 0.0f;
      pClearColor->g = 0.7490196078431373f;
      pClearColor->b = 1.0f;
      break;

    case eColor::Dimgray:
      pClearColor->r = 0.4117647058823529f;
      pClearColor->g = 0.4117647058823529f;
      pClearColor->b = 0.4117647058823529f;
      break;

    case eColor::Dodgerblue:
      pClearColor->r = 0.11764705882352941f;
      pClearColor->g = 0.5647058823529412f;
      pClearColor->b = 1.0f;
      break;

    case eColor::Firebrick:
      pClearColor->r = 0.6980392156862745f;
      pClearColor->g = 0.13333333333333333f;
      pClearColor->b = 0.13333333333333333f;
      break;

    case eColor::Floralwhite:
      pClearColor->r = 1.0f;
      pClearColor->g = 0.9803921568627451f;
      pClearColor->b = 0.9411764705882353f;
      break;

    case eColor::Forestgreen:
      pClearColor->r = 0.13333333333333333f;
      pClearColor->g = 0.5450980392156862f;
      pClearColor->b = 0.13333333333333333f;
      break;

    case eColor::Gainsboro:
      pClearColor->r = 0.8627450980392157f;
      pClearColor->g = 0.8627450980392157f;
      pClearColor->b = 0.8627450980392157f;
      break;

    case eColor::Ghostwhite:
      pClearColor->r = 0.9725490196078431f;
      pClearColor->g = 0.9725490196078431f;
      pClearColor->b = 1.0f;
      break;

    case eColor::Gold:
      pClearColor->r = 1.0f;
      pClearColor->g = 0.8431372549019608f;
      pClearColor->b = 0.0f;
      break;

    case eColor::Goldenrod:
      pClearColor->r = 0.8549019607843137f;
      pClearColor->g = 0.6470588235294118f;
      pClearColor->b = 0.12549019607843137f;
      break;

    case eColor::Greenyellow:
      pClearColor->r = 0.6784313725490196f;
      pClearColor->g = 1.0f;
      pClearColor->b = 0.1843137254901961f;
      break;

    case eColor::Honeydew:
      pClearColor->r = 0.9411764705882353f;
      pClearColor->g = 1.0f;
      pClearColor->b = 0.9411764705882353f;
      break;

    case eColor::Hotpink:
      pClearColor->r = 1.0f;
      pClearColor->g = 0.4117647058823529f;
      pClearColor->b = 0.7058823529411765f;
      break;

    case eColor::Indianred:
      pClearColor->r = 0.803921568627451f;
      pClearColor->g = 0.3607843137254902f;
      pClearColor->b = 0.3607843137254902f;
      break;

    case eColor::Indigo:
      pClearColor->r = 0.29411764705882354f;
      pClearColor->g = 0.0f;
      pClearColor->b = 0.5098039215686274f;
      break;

    case eColor::Ivory:
      pClearColor->r = 1.0f;
      pClearColor->g = 1.0f;
      pClearColor->b = 0.9411764705882353f;
      break;

    case eColor::Khaki:
      pClearColor->r = 0.9411764705882353f;
      pClearColor->g = 0.9019607843137255f;
      pClearColor->b = 0.5490196078431373f;
      break;

    case eColor::Lavender:
      pClearColor->r = 0.9019607843137255f;
      pClearColor->g = 0.9019607843137255f;
      pClearColor->b = 0.9803921568627451f;
      break;

    case eColor::Lavenderblush:
      pClearColor->r = 1.0f;
      pClearColor->g = 0.9411764705882353f;
      pClearColor->b = 0.9607843137254902f;
      break;

    case eColor::Lawngreen:
      pClearColor->r = 0.48627450980392156f;
      pClearColor->g = 0.9882352941176471f;
      pClearColor->b = 0.0f;
      break;

    case eColor::Lemonchiffon:
      pClearColor->r = 1.0f;
      pClearColor->g = 0.9803921568627451f;
      pClearColor->b = 0.803921568627451f;
      break;

    case eColor::Lightblue:
      pClearColor->r = 0.6784313725490196f;
      pClearColor->g = 0.8470588235294118f;
      pClearColor->b = 0.9019607843137255f;
      break;

    case eColor::Lightcoral:
      pClearColor->r = 0.9411764705882353f;
      pClearColor->g = 0.5019607843137255f;
      pClearColor->b = 0.5019607843137255f;
      break;

    case eColor::Lightcyan:
      pClearColor->r = 0.8784313725490196f;
      pClearColor->g = 1.0f;
      pClearColor->b = 1.0f;
      break;

    case eColor::Lightgoldenrodyellow:
      pClearColor->r = 0.9803921568627451f;
      pClearColor->g = 0.9803921568627451f;
      pClearColor->b = 0.8235294117647058f;
      break;

    case eColor::Lightgray:
      pClearColor->r = 0.8274509803921568f;
      pClearColor->g = 0.8274509803921568f;
      pClearColor->b = 0.8274509803921568f;
      break;

    case eColor::Lightgreen:
      pClearColor->r = 0.5647058823529412f;
      pClearColor->g = 0.9333333333333333f;
      pClearColor->b = 0.5647058823529412f;
      break;

    case eColor::Lightpink:
      pClearColor->r = 1.0f;
      pClearColor->g = 0.7137254901960784f;
      pClearColor->b = 0.7568627450980392f;
      break;

    case eColor::Lightsalmon:
      pClearColor->r = 1.0f;
      pClearColor->g = 0.6274509803921569f;
      pClearColor->b = 0.47843137254901963f;
      break;

    case eColor::Lightseagreen:
      pClearColor->r = 0.12549019607843137f;
      pClearColor->g = 0.6980392156862745f;
      pClearColor->b = 0.6666666666666666f;
      break;

    case eColor::Lightskyblue:
      pClearColor->r = 0.5294117647058824f;
      pClearColor->g = 0.807843137254902f;
      pClearColor->b = 0.9803921568627451f;
      break;

    case eColor::Lightslategray:
      pClearColor->r = 0.4666666666666667f;
      pClearColor->g = 0.5333333333333333f;
      pClearColor->b = 0.6f;
      break;

    case eColor::Lightsteelblue:
      pClearColor->r = 0.6901960784313725f;
      pClearColor->g = 0.7686274509803922f;
      pClearColor->b = 0.8705882352941177f;
      break;

    case eColor::Lightyellow:
      pClearColor->r = 1.0f;
      pClearColor->g = 1.0f;
      pClearColor->b = 0.8784313725490196f;
      break;

    case eColor::Limegreen:
      pClearColor->r = 0.19607843137254902f;
      pClearColor->g = 0.803921568627451f;
      pClearColor->b = 0.19607843137254902f;
      break;

    case eColor::Linen:
      pClearColor->r = 0.9803921568627451f;
      pClearColor->g = 0.9411764705882353f;
      pClearColor->b = 0.9019607843137255f;
      break;

    case eColor::Magenta:
      pClearColor->r = 1.0f;
      pClearColor->g = 0.0f;
      pClearColor->b = 1.0f;
      break;

    case eColor::Mediumaquamarine:
      pClearColor->r = 0.4f;
      pClearColor->g = 0.803921568627451f;
      pClearColor->b = 0.6666666666666666f;
      break;

    case eColor::Mediumblue:
      pClearColor->r = 0.0f;
      pClearColor->g = 0.0f;
      pClearColor->b = 0.803921568627451f;
      break;

    case eColor::Mediumorchid:
      pClearColor->r = 0.7294117647058823f;
      pClearColor->g = 0.3333333333333333f;
      pClearColor->b = 0.8274509803921568f;
      break;

    case eColor::Mediumpurple:
      pClearColor->r = 0.5764705882352941f;
      pClearColor->g = 0.4392156862745098f;
      pClearColor->b = 0.8588235294117647f;
      break;

    case eColor::Mediumseagreen:
      pClearColor->r = 0.23529411764705882f;
      pClearColor->g = 0.7019607843137254f;
      pClearColor->b = 0.44313725490196076f;
      break;

    case eColor::Mediumslateblue:
      pClearColor->r = 0.4823529411764706f;
      pClearColor->g = 0.40784313725490196f;
      pClearColor->b = 0.9333333333333333f;
      break;

    case eColor::Mediumspringgreen:
      pClearColor->r = 0.0f;
      pClearColor->g = 0.9803921568627451f;
      pClearColor->b = 0.6039215686274509f;
      break;

    case eColor::Mediumturquoise:
      pClearColor->r = 0.2823529411764706f;
      pClearColor->g = 0.8196078431372549f;
      pClearColor->b = 0.8f;
      break;

    case eColor::Mediumvioletred:
      pClearColor->r = 0.7803921568627451f;
      pClearColor->g = 0.08235294117647059f;
      pClearColor->b = 0.5215686274509804f;
      break;

    case eColor::Midnightblue:
      pClearColor->r = 0.09803921568627451f;
      pClearColor->g = 0.09803921568627451f;
      pClearColor->b = 0.4392156862745098f;
      break;

    case eColor::Mintcream:
      pClearColor->r = 0.9607843137254902f;
      pClearColor->g = 1.0f;
      pClearColor->b = 0.9803921568627451f;
      break;

    case eColor::Mistyrose:
      pClearColor->r = 1.0f;
      pClearColor->g = 0.8941176470588236f;
      pClearColor->b = 0.8823529411764706f;
      break;

    case eColor::Moccasin:
      pClearColor->r = 1.0f;
      pClearColor->g = 0.8941176470588236f;
      pClearColor->b = 0.7098039215686275f;
      break;

    case eColor::Navajowhite:
      pClearColor->r = 1.0f;
      pClearColor->g = 0.8705882352941177f;
      pClearColor->b = 0.6784313725490196f;
      break;

    case eColor::Oldlace:
      pClearColor->r = 0.9921568627450981f;
      pClearColor->g = 0.9607843137254902f;
      pClearColor->b = 0.9019607843137255f;
      break;

    case eColor::Olivedrab:
      pClearColor->r = 0.4196078431372549f;
      pClearColor->g = 0.5568627450980392f;
      pClearColor->b = 0.13725490196078433f;
      break;

    case eColor::Orange:
      pClearColor->r = 1.0f;
      pClearColor->g = 0.6470588235294118f;
      pClearColor->b = 0.0f;
      break;

    case eColor::Orangered:
      pClearColor->r = 1.0f;
      pClearColor->g = 0.27058823529411763f;
      pClearColor->b = 0.0f;
      break;

    case eColor::Orchid:
      pClearColor->r = 0.8549019607843137f;
      pClearColor->g = 0.4392156862745098f;
      pClearColor->b = 0.8392156862745098f;
      break;

    case eColor::Palegoldenrod:
      pClearColor->r = 0.9333333333333333f;
      pClearColor->g = 0.9098039215686274f;
      pClearColor->b = 0.6666666666666666f;
      break;

    case eColor::Palegreen:
      pClearColor->r = 0.596078431372549f;
      pClearColor->g = 0.984313725490196f;
      pClearColor->b = 0.596078431372549f;
      break;

    case eColor::Paleturquoise:
      pClearColor->r = 0.6862745098039216f;
      pClearColor->g = 0.9333333333333333f;
      pClearColor->b = 0.9333333333333333f;
      break;

    case eColor::Palevioletred:
      pClearColor->r = 0.8588235294117647f;
      pClearColor->g = 0.4392156862745098f;
      pClearColor->b = 0.5764705882352941f;
      break;

    case eColor::Papayawhip:
      pClearColor->r = 1.0f;
      pClearColor->g = 0.9372549019607843f;
      pClearColor->b = 0.8352941176470589f;
      break;

    case eColor::Peachpuff:
      pClearColor->r = 1.0f;
      pClearColor->g = 0.8549019607843137f;
      pClearColor->b = 0.7254901960784313f;
      break;

    case eColor::Peru:
      pClearColor->r = 0.803921568627451f;
      pClearColor->g = 0.5215686274509804f;
      pClearColor->b = 0.24705882352941178f;
      break;

    case eColor::Pink:
      pClearColor->r = 1.0f;
      pClearColor->g = 0.7529411764705882f;
      pClearColor->b = 0.796078431372549f;
      break;

    case eColor::Plum:
      pClearColor->r = 0.8666666666666667f;
      pClearColor->g = 0.6274509803921569f;
      pClearColor->b = 0.8666666666666667f;
      break;

    case eColor::Powderblue:
      pClearColor->r = 0.6901960784313725f;
      pClearColor->g = 0.8784313725490196f;
      pClearColor->b = 0.9019607843137255f;
      break;

    case eColor::Rosybrown:
      pClearColor->r = 0.7372549019607844f;
      pClearColor->g = 0.5607843137254902f;
      pClearColor->b = 0.5607843137254902f;
      break;

    case eColor::Royalblue:
      pClearColor->r = 0.2549019607843137f;
      pClearColor->g = 0.4117647058823529f;
      pClearColor->b = 0.8823529411764706f;
      break;

    case eColor::Saddlebrown:
      pClearColor->r = 0.5450980392156862f;
      pClearColor->g = 0.27058823529411763f;
      pClearColor->b = 0.07450980392156863f;
      break;

    case eColor::Salmon:
      pClearColor->r = 0.9803921568627451f;
      pClearColor->g = 0.5019607843137255f;
      pClearColor->b = 0.4470588235294118f;
      break;

    case eColor::Sandybrown:
      pClearColor->r = 0.9568627450980393f;
      pClearColor->g = 0.6431372549019608f;
      pClearColor->b = 0.3764705882352941f;
      break;

    case eColor::Seagreen:
      pClearColor->r = 0.1803921568627451f;
      pClearColor->g = 0.5450980392156862f;
      pClearColor->b = 0.3411764705882353f;
      break;

    case eColor::Seashell:
      pClearColor->r = 1.0f;
      pClearColor->g = 0.9607843137254902f;
      pClearColor->b = 0.9333333333333333f;
      break;

    case eColor::Sienna:
      pClearColor->r = 0.6274509803921569f;
      pClearColor->g = 0.3215686274509804f;
      pClearColor->b = 0.17647058823529413f;
      break;

    case eColor::Skyblue:
      pClearColor->r = 0.5294117647058824f;
      pClearColor->g = 0.807843137254902f;
      pClearColor->b = 0.9215686274509803f;
      break;

    case eColor::Slateblue:
      pClearColor->r = 0.41568627450980394f;
      pClearColor->g = 0.35294117647058826f;
      pClearColor->b = 0.803921568627451f;
      break;

    case eColor::Slategray:
      pClearColor->r = 0.4392156862745098f;
      pClearColor->g = 0.5019607843137255f;
      pClearColor->b = 0.5647058823529412f;
      break;

    case eColor::Snow:
      pClearColor->r = 1.0f;
      pClearColor->g = 0.9803921568627451f;
      pClearColor->b = 0.9803921568627451f;
      break;

    case eColor::Springgreen:
      pClearColor->r = 0.0f;
      pClearColor->g = 1.0f;
      pClearColor->b = 0.4980392156862745f;
      break;

    case eColor::Steelblue:
        pClearColor->r = 0.27450980392156865f;
        pClearColor->g = 0.5098039215686274f;
        pClearColor->b = 0.7058823529411765f;
        break;

    case eColor::Tan:
        pClearColor->r = 0.8235294117647058f;
        pClearColor->g = 0.7058823529411765f;
        pClearColor->b = 0.5490196078431373f;
        break;

    case eColor::Thistle:
        pClearColor->r = 0.8470588235294118f;
        pClearColor->g = 0.7490196078431373f;
        pClearColor->b = 0.8470588235294118f;
        break;

    case eColor::Tomato:
        pClearColor->r = 1.0f;
        pClearColor->g = 0.38823529411764707f;
        pClearColor->b = 0.2784313725490196f;
        break;

    case eColor::Turquoise:
        pClearColor->r = 0.25098039215686274f;
        pClearColor->g = 0.8784313725490196f;
        pClearColor->b = 0.8156862745098039f;
        break;

    case eColor::Violet:
        pClearColor->r = 0.9333333333333333f;
        pClearColor->g = 0.5098039215686274f;
        pClearColor->b = 0.9333333333333333f;
        break;

    case eColor::Wheat:
        pClearColor->r = 0.9607843137254902f;
        pClearColor->g = 0.8705882352941177f;
        pClearColor->b = 0.7019607843137254f;
        break;

    case eColor::Whitesmoke:
        pClearColor->r = 0.9607843137254902f;
        pClearColor->g = 0.9607843137254902f;
        pClearColor->b = 0.9607843137254902f;
        break;

    case eColor::Yellowgreen:
        pClearColor->r = 0.6039215686274509f;
        pClearColor->g = 0.803921568627451f;
        pClearColor->b = 0.19607843137254902f;
        break;
    }
}
