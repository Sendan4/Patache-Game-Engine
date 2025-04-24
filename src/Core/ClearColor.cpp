#include <SDL3/SDL.h>

#include "PatataEngine/PatataEngine.hpp"

void
Patata::Engine::ClearColorRGBA (const float & R, const float & G,
                                const float & B, const float & A)
{
  clearColor.r = R;
  clearColor.g = G;
  clearColor.b = B;
  clearColor.a = A;
}

void
Patata::Engine::ClearColorRGBA (const float & R, const float & G,
                                const float & B)
{
  clearColor.r = R;
  clearColor.g = G;
  clearColor.b = B;
}

void
Patata::Engine::ClearColor (const Color & Color)
{
  switch (Color)
    {
    // Basic Colors
    case Color::Black:
      clearColor.r = 0.0f;
      clearColor.g = 0.0f;
      clearColor.b = 0.0f;
      break;

    case Color::Silver:
      clearColor.r = 0.7529411764705882f;
      clearColor.g = 0.7529411764705882f;
      clearColor.b = 0.7529411764705882f;
      break;

    case Color::Gray:
      clearColor.r = 0.5019607843137255f;
      clearColor.g = 0.5019607843137255f;
      clearColor.b = 0.5019607843137255f;
      break;

    case Color::White:
      clearColor.r = 1.0f;
      clearColor.g = 1.0f;
      clearColor.b = 1.0f;
      break;

    case Color::Maroon:
      clearColor.r = 0.5019607843137255f;
      clearColor.g = 0.0f;
      clearColor.b = 0.0f;
      break;

    case Color::Red:
      clearColor.r = 1.0f;
      clearColor.g = 0.0f;
      clearColor.b = 0.0f;
      break;

    case Color::Purple:
      clearColor.r = 0.5019607843137255f;
      clearColor.g = 0.0f;
      clearColor.b = 0.5019607843137255f;
      break;

    case Color::Fuchsia:
      clearColor.r = 1.0f;
      clearColor.g = 0.0f;
      clearColor.b = 1.0f;
      break;

    case Color::Green:
      clearColor.r = 0.0f;
      clearColor.g = 0.5019607843137255f;
      clearColor.b = 0.0f;
      break;

    case Color::Lime:
      clearColor.r = 0.0f;
      clearColor.g = 1.0f;
      clearColor.b = 0.0f;
      break;

    case Color::Olive:
      clearColor.r = 0.5019607843137255f;
      clearColor.g = 0.5019607843137255f;
      clearColor.b = 0.0f;
      break;

    case Color::Yellow:
      clearColor.r = 1.0f;
      clearColor.g = 1.0f;
      clearColor.b = 0.0f;
      break;

    case Color::Navy:
      clearColor.r = 0.0f;
      clearColor.g = 0.0f;
      clearColor.b = 0.5019607843137255f;
      break;

    case Color::Blue:
      clearColor.r = 0.0f;
      clearColor.g = 0.0f;
      clearColor.b = 1.0f;
      break;

    case Color::Teal:
      clearColor.r = 0.0f;
      clearColor.g = 0.5019607843137255f;
      clearColor.b = 0.5019607843137255f;
      break;

    case Color::Aqua:
      clearColor.r = 0.0f;
      clearColor.g = 1.0f;
      clearColor.b = 1.0f;
      break;

    // Extended colors
    case Color::Aliceblue:
      clearColor.r = 0.9411764705882353f;
      clearColor.g = 0.9725490196078431f;
      clearColor.b = 1.0f;
      break;

    case Color::Antiquewhite:
      clearColor.r = 0.9803921568627451f;
      clearColor.g = 0.9215686274509803f;
      clearColor.b = 0.8431372549019608f;
      break;

    case Color::Aquamarine:
      clearColor.r = 0.4980392156862745f;
      clearColor.g = 1.0f;
      clearColor.b = 0.8313725490196079f;
      break;

    case Color::Azure:
      clearColor.r = 0.9411764705882353f;
      clearColor.g = 1.0f;
      clearColor.b = 1.0f;
      break;

    case Color::Beige:
      clearColor.r = 0.9607843137254902f;
      clearColor.g = 0.9607843137254902f;
      clearColor.b = 0.8627450980392157f;
      break;

    case Color::Bisque:
      clearColor.r = 1.0f;
      clearColor.g = 0.8941176470588236f;
      clearColor.b = 0.7686274509803922f;
      break;

    case Color::Blanchedalmond:
      clearColor.r = 1.0f;
      clearColor.g = 0.9215686274509803f;
      clearColor.b = 0.803921568627451f;
      break;

    case Color::Blueviolet:
      clearColor.r = 0.5411764705882353f;
      clearColor.g = 0.16862745098039217f;
      clearColor.b = 0.8862745098039215f;
      break;

    case Color::Brown:
      clearColor.r = 0.6470588235294118f;
      clearColor.g = 0.16470588235294117f;
      clearColor.b = 0.16470588235294117f;
      break;

    case Color::Burlywood:
      clearColor.r = 0.8705882352941177f;
      clearColor.g = 0.7215686274509804f;
      clearColor.b = 0.5294117647058824f;
      break;

    case Color::Cadetblue:
      clearColor.r = 0.37254901960784315f;
      clearColor.g = 0.6196078431372549f;
      clearColor.b = 0.6274509803921569f;
      break;

    case Color::Chartreuse:
      clearColor.r = 0.4980392156862745f;
      clearColor.g = 1.0f;
      clearColor.b = 0.0f;
      break;

    case Color::Chocolate:
      clearColor.r = 0.8235294117647058f;
      clearColor.g = 0.4117647058823529f;
      clearColor.b = 0.11764705882352941f;
      break;

    case Color::Coral:
      clearColor.r = 1.0f;
      clearColor.g = 0.4980392156862745f;
      clearColor.b = 0.3137254901960784f;
      break;

    case Color::Cornflowerblue:
      clearColor.r = 0.39215686274509803f;
      clearColor.g = 0.5843137254901961f;
      clearColor.b = 0.9294117647058824f;
      break;

    case Color::Cornsilk:
      clearColor.r = 1.0f;
      clearColor.g = 0.9725490196078431f;
      clearColor.b = 0.8627450980392157f;
      break;

    case Color::Crimson:
      clearColor.r = 0.8627450980392157f;
      clearColor.g = 0.0784313725490196f;
      clearColor.b = 0.23529411764705882f;
      break;

    case Color::Cyan:
      clearColor.r = 0.0f;
      clearColor.g = 1.0f;
      clearColor.b = 1.0f;
      break;

    case Color::Darkblue:
      clearColor.r = 0.0f;
      clearColor.g = 0.0f;
      clearColor.b = 0.5450980392156862f;
      break;

    case Color::Darkcyan:
      clearColor.r = 0.0f;
      clearColor.g = 0.5450980392156862f;
      clearColor.b = 0.5450980392156862f;
      break;

    case Color::Darkgoldenrod:
      clearColor.r = 0.7215686274509804f;
      clearColor.g = 0.5254901960784314f;
      clearColor.b = 0.043137254901960784f;
      break;

    case Color::Darkgray:
      clearColor.r = 0.6627450980392157f;
      clearColor.g = 0.6627450980392157f;
      clearColor.b = 0.6627450980392157f;
      break;

    case Color::Darkgreen:
      clearColor.r = 0.0f;
      clearColor.g = 0.39215686274509803f;
      clearColor.b = 0.0f;
      break;

    case Color::Darkkhaki:
      clearColor.r = 0.7411764705882353f;
      clearColor.g = 0.7176470588235294f;
      clearColor.b = 0.4196078431372549f;
      break;

    case Color::Darkmagenta:
      clearColor.r = 0.5450980392156862f;
      clearColor.g = 0.0f;
      clearColor.b = 0.5450980392156862f;
      break;

    case Color::Darkolivegreen:
      clearColor.r = 0.3333333333333333f;
      clearColor.g = 0.4196078431372549f;
      clearColor.b = 0.1843137254901961f;
      break;

    case Color::Darkorange:
      clearColor.r = 1.0f;
      clearColor.g = 0.5490196078431373f;
      clearColor.b = 0.0f;
      break;

    case Color::Darkorchid:
      clearColor.r = 0.6f;
      clearColor.g = 0.19607843137254902f;
      clearColor.b = 0.8f;
      break;

    case Color::Darkred:
      clearColor.r = 0.5450980392156862f;
      clearColor.g = 0.0f;
      clearColor.b = 0.0f;
      break;

    case Color::Darksalmon:
      clearColor.r = 0.9137254901960784f;
      clearColor.g = 0.5882352941176471f;
      clearColor.b = 0.47843137254901963f;
      break;

    case Color::Darkseagreen:
      clearColor.r = 0.5607843137254902f;
      clearColor.g = 0.7372549019607844f;
      clearColor.b = 0.5607843137254902f;
      break;

    case Color::Darkslateblue:
      clearColor.r = 0.2823529411764706f;
      clearColor.g = 0.23921568627450981f;
      clearColor.b = 0.5450980392156862f;
      break;

    case Color::Darkslategray:
      clearColor.r = 0.1843137254901961f;
      clearColor.g = 0.30980392156862746f;
      clearColor.b = 0.30980392156862746f;
      break;

    case Color::Darkturquoise:
      clearColor.r = 0.0f;
      clearColor.g = 0.807843137254902f;
      clearColor.b = 0.8196078431372549f;
      break;

    case Color::Darkviolet:
      clearColor.r = 0.5803921568627451f;
      clearColor.g = 0.0f;
      clearColor.b = 0.8274509803921568f;
      break;

    case Color::Deeppink:
      clearColor.r = 1.0f;
      clearColor.g = 0.0784313725490196f;
      clearColor.b = 0.5764705882352941f;
      break;

    case Color::Deepskyblue:
      clearColor.r = 0.0f;
      clearColor.g = 0.7490196078431373f;
      clearColor.b = 1.0f;
      break;

    case Color::Dimgray:
      clearColor.r = 0.4117647058823529f;
      clearColor.g = 0.4117647058823529f;
      clearColor.b = 0.4117647058823529f;
      break;

    case Color::Dodgerblue:
      clearColor.r = 0.11764705882352941f;
      clearColor.g = 0.5647058823529412f;
      clearColor.b = 1.0f;
      break;

    case Color::Firebrick:
      clearColor.r = 0.6980392156862745f;
      clearColor.g = 0.13333333333333333f;
      clearColor.b = 0.13333333333333333f;
      break;

    case Color::Floralwhite:
      clearColor.r = 1.0f;
      clearColor.g = 0.9803921568627451f;
      clearColor.b = 0.9411764705882353f;
      break;

    case Color::Forestgreen:
      clearColor.r = 0.13333333333333333f;
      clearColor.g = 0.5450980392156862f;
      clearColor.b = 0.13333333333333333f;
      break;

    case Color::Gainsboro:
      clearColor.r = 0.8627450980392157f;
      clearColor.g = 0.8627450980392157f;
      clearColor.b = 0.8627450980392157f;
      break;

    case Color::Ghostwhite:
      clearColor.r = 0.9725490196078431f;
      clearColor.g = 0.9725490196078431f;
      clearColor.b = 1.0f;
      break;

    case Color::Gold:
      clearColor.r = 1.0f;
      clearColor.g = 0.8431372549019608f;
      clearColor.b = 0.0f;
      break;

    case Color::Goldenrod:
      clearColor.r = 0.8549019607843137f;
      clearColor.g = 0.6470588235294118f;
      clearColor.b = 0.12549019607843137f;
      break;

    case Color::Greenyellow:
      clearColor.r = 0.6784313725490196f;
      clearColor.g = 1.0f;
      clearColor.b = 0.1843137254901961f;
      break;

    case Color::Honeydew:
      clearColor.r = 0.9411764705882353f;
      clearColor.g = 1.0f;
      clearColor.b = 0.9411764705882353f;
      break;

    case Color::Hotpink:
      clearColor.r = 1.0f;
      clearColor.g = 0.4117647058823529f;
      clearColor.b = 0.7058823529411765f;
      break;

    case Color::Indianred:
      clearColor.r = 0.803921568627451f;
      clearColor.g = 0.3607843137254902f;
      clearColor.b = 0.3607843137254902f;
      break;

    case Color::Indigo:
      clearColor.r = 0.29411764705882354f;
      clearColor.g = 0.0f;
      clearColor.b = 0.5098039215686274f;
      break;

    case Color::Ivory:
      clearColor.r = 1.0f;
      clearColor.g = 1.0f;
      clearColor.b = 0.9411764705882353f;
      break;

    case Color::Khaki:
      clearColor.r = 0.9411764705882353f;
      clearColor.g = 0.9019607843137255f;
      clearColor.b = 0.5490196078431373f;
      break;

    case Color::Lavender:
      clearColor.r = 0.9019607843137255f;
      clearColor.g = 0.9019607843137255f;
      clearColor.b = 0.9803921568627451f;
      break;

    case Color::Lavenderblush:
      clearColor.r = 1.0f;
      clearColor.g = 0.9411764705882353f;
      clearColor.b = 0.9607843137254902f;
      break;

    case Color::Lawngreen:
      clearColor.r = 0.48627450980392156f;
      clearColor.g = 0.9882352941176471f;
      clearColor.b = 0.0f;
      break;

    case Color::Lemonchiffon:
      clearColor.r = 1.0f;
      clearColor.g = 0.9803921568627451f;
      clearColor.b = 0.803921568627451f;
      break;

    case Color::Lightblue:
      clearColor.r = 0.6784313725490196f;
      clearColor.g = 0.8470588235294118f;
      clearColor.b = 0.9019607843137255f;
      break;

    case Color::Lightcoral:
      clearColor.r = 0.9411764705882353f;
      clearColor.g = 0.5019607843137255f;
      clearColor.b = 0.5019607843137255f;
      break;

    case Color::Lightcyan:
      clearColor.r = 0.8784313725490196f;
      clearColor.g = 1.0f;
      clearColor.b = 1.0f;
      break;

    case Color::Lightgoldenrodyellow:
      clearColor.r = 0.9803921568627451f;
      clearColor.g = 0.9803921568627451f;
      clearColor.b = 0.8235294117647058f;
      break;

    case Color::Lightgray:
      clearColor.r = 0.8274509803921568f;
      clearColor.g = 0.8274509803921568f;
      clearColor.b = 0.8274509803921568f;
      break;

    case Color::Lightgreen:
      clearColor.r = 0.5647058823529412f;
      clearColor.g = 0.9333333333333333f;
      clearColor.b = 0.5647058823529412f;
      break;

    case Color::Lightpink:
      clearColor.r = 1.0f;
      clearColor.g = 0.7137254901960784f;
      clearColor.b = 0.7568627450980392f;
      break;

    case Color::Lightsalmon:
      clearColor.r = 1.0f;
      clearColor.g = 0.6274509803921569f;
      clearColor.b = 0.47843137254901963f;
      break;

    case Color::Lightseagreen:
      clearColor.r = 0.12549019607843137f;
      clearColor.g = 0.6980392156862745f;
      clearColor.b = 0.6666666666666666f;
      break;

    case Color::Lightskyblue:
      clearColor.r = 0.5294117647058824f;
      clearColor.g = 0.807843137254902f;
      clearColor.b = 0.9803921568627451f;
      break;

    case Color::Lightslategray:
      clearColor.r = 0.4666666666666667f;
      clearColor.g = 0.5333333333333333f;
      clearColor.b = 0.6f;
      break;

    case Color::Lightsteelblue:
      clearColor.r = 0.6901960784313725f;
      clearColor.g = 0.7686274509803922f;
      clearColor.b = 0.8705882352941177f;
      break;

    case Color::Lightyellow:
      clearColor.r = 1.0f;
      clearColor.g = 1.0f;
      clearColor.b = 0.8784313725490196f;
      break;

    case Color::Limegreen:
      clearColor.r = 0.19607843137254902f;
      clearColor.g = 0.803921568627451f;
      clearColor.b = 0.19607843137254902f;
      break;

    case Color::Linen:
      clearColor.r = 0.9803921568627451f;
      clearColor.g = 0.9411764705882353f;
      clearColor.b = 0.9019607843137255f;
      break;

    case Color::Magenta:
      clearColor.r = 1.0f;
      clearColor.g = 0.0f;
      clearColor.b = 1.0f;
      break;

    case Color::Mediumaquamarine:
      clearColor.r = 0.4f;
      clearColor.g = 0.803921568627451f;
      clearColor.b = 0.6666666666666666f;
      break;

    case Color::Mediumblue:
      clearColor.r = 0.0f;
      clearColor.g = 0.0f;
      clearColor.b = 0.803921568627451f;
      break;

    case Color::Mediumorchid:
      clearColor.r = 0.7294117647058823f;
      clearColor.g = 0.3333333333333333f;
      clearColor.b = 0.8274509803921568f;
      break;

    case Color::Mediumpurple:
      clearColor.r = 0.5764705882352941f;
      clearColor.g = 0.4392156862745098f;
      clearColor.b = 0.8588235294117647f;
      break;

    case Color::Mediumseagreen:
      clearColor.r = 0.23529411764705882f;
      clearColor.g = 0.7019607843137254f;
      clearColor.b = 0.44313725490196076f;
      break;

    case Color::Mediumslateblue:
      clearColor.r = 0.4823529411764706f;
      clearColor.g = 0.40784313725490196f;
      clearColor.b = 0.9333333333333333f;
      break;

    case Color::Mediumspringgreen:
      clearColor.r = 0.0f;
      clearColor.g = 0.9803921568627451f;
      clearColor.b = 0.6039215686274509f;
      break;

    case Color::Mediumturquoise:
      clearColor.r = 0.2823529411764706f;
      clearColor.g = 0.8196078431372549f;
      clearColor.b = 0.8f;
      break;

    case Color::Mediumvioletred:
      clearColor.r = 0.7803921568627451f;
      clearColor.g = 0.08235294117647059f;
      clearColor.b = 0.5215686274509804f;
      break;

    case Color::Midnightblue:
      clearColor.r = 0.09803921568627451f;
      clearColor.g = 0.09803921568627451f;
      clearColor.b = 0.4392156862745098f;
      break;

    case Color::Mintcream:
      clearColor.r = 0.9607843137254902f;
      clearColor.g = 1.0f;
      clearColor.b = 0.9803921568627451f;
      break;

    case Color::Mistyrose:
      clearColor.r = 1.0f;
      clearColor.g = 0.8941176470588236f;
      clearColor.b = 0.8823529411764706f;
      break;

    case Color::Moccasin:
      clearColor.r = 1.0f;
      clearColor.g = 0.8941176470588236f;
      clearColor.b = 0.7098039215686275f;
      break;

    case Color::Navajowhite:
      clearColor.r = 1.0f;
      clearColor.g = 0.8705882352941177f;
      clearColor.b = 0.6784313725490196f;
      break;

    case Color::Oldlace:
      clearColor.r = 0.9921568627450981f;
      clearColor.g = 0.9607843137254902f;
      clearColor.b = 0.9019607843137255f;
      break;

    case Color::Olivedrab:
      clearColor.r = 0.4196078431372549f;
      clearColor.g = 0.5568627450980392f;
      clearColor.b = 0.13725490196078433f;
      break;

    case Color::Orange:
      clearColor.r = 1.0f;
      clearColor.g = 0.6470588235294118f;
      clearColor.b = 0.0f;
      break;

    case Color::Orangered:
      clearColor.r = 1.0f;
      clearColor.g = 0.27058823529411763f;
      clearColor.b = 0.0f;
      break;

    case Color::Orchid:
      clearColor.r = 0.8549019607843137f;
      clearColor.g = 0.4392156862745098f;
      clearColor.b = 0.8392156862745098f;
      break;

    case Color::Palegoldenrod:
      clearColor.r = 0.9333333333333333f;
      clearColor.g = 0.9098039215686274f;
      clearColor.b = 0.6666666666666666f;
      break;

    case Color::Palegreen:
      clearColor.r = 0.596078431372549f;
      clearColor.g = 0.984313725490196f;
      clearColor.b = 0.596078431372549f;
      break;

    case Color::Paleturquoise:
      clearColor.r = 0.6862745098039216f;
      clearColor.g = 0.9333333333333333f;
      clearColor.b = 0.9333333333333333f;
      break;

    case Color::Palevioletred:
      clearColor.r = 0.8588235294117647f;
      clearColor.g = 0.4392156862745098f;
      clearColor.b = 0.5764705882352941f;
      break;

    case Color::Papayawhip:
      clearColor.r = 1.0f;
      clearColor.g = 0.9372549019607843f;
      clearColor.b = 0.8352941176470589f;
      break;

    case Color::Peachpuff:
      clearColor.r = 1.0f;
      clearColor.g = 0.8549019607843137f;
      clearColor.b = 0.7254901960784313f;
      break;

    case Color::Peru:
      clearColor.r = 0.803921568627451f;
      clearColor.g = 0.5215686274509804f;
      clearColor.b = 0.24705882352941178f;
      break;

    case Color::Pink:
      clearColor.r = 1.0f;
      clearColor.g = 0.7529411764705882f;
      clearColor.b = 0.796078431372549f;
      break;

    case Color::Plum:
      clearColor.r = 0.8666666666666667f;
      clearColor.g = 0.6274509803921569f;
      clearColor.b = 0.8666666666666667f;
      break;

    case Color::Powderblue:
      clearColor.r = 0.6901960784313725f;
      clearColor.g = 0.8784313725490196f;
      clearColor.b = 0.9019607843137255f;
      break;

    case Color::Rosybrown:
      clearColor.r = 0.7372549019607844f;
      clearColor.g = 0.5607843137254902f;
      clearColor.b = 0.5607843137254902f;
      break;

    case Color::Royalblue:
      clearColor.r = 0.2549019607843137f;
      clearColor.g = 0.4117647058823529f;
      clearColor.b = 0.8823529411764706f;
      break;

    case Color::Saddlebrown:
      clearColor.r = 0.5450980392156862f;
      clearColor.g = 0.27058823529411763f;
      clearColor.b = 0.07450980392156863f;
      break;

    case Color::Salmon:
      clearColor.r = 0.9803921568627451f;
      clearColor.g = 0.5019607843137255f;
      clearColor.b = 0.4470588235294118f;
      break;

    case Color::Sandybrown:
      clearColor.r = 0.9568627450980393f;
      clearColor.g = 0.6431372549019608f;
      clearColor.b = 0.3764705882352941f;
      break;

    case Color::Seagreen:
      clearColor.r = 0.1803921568627451f;
      clearColor.g = 0.5450980392156862f;
      clearColor.b = 0.3411764705882353f;
      break;

    case Color::Seashell:
      clearColor.r = 1.0f;
      clearColor.g = 0.9607843137254902f;
      clearColor.b = 0.9333333333333333f;
      break;

    case Color::Sienna:
      clearColor.r = 0.6274509803921569f;
      clearColor.g = 0.3215686274509804f;
      clearColor.b = 0.17647058823529413f;
      break;

    case Color::Skyblue:
      clearColor.r = 0.5294117647058824f;
      clearColor.g = 0.807843137254902f;
      clearColor.b = 0.9215686274509803f;
      break;

    case Color::Slateblue:
      clearColor.r = 0.41568627450980394f;
      clearColor.g = 0.35294117647058826f;
      clearColor.b = 0.803921568627451f;
      break;

    case Color::Slategray:
      clearColor.r = 0.4392156862745098f;
      clearColor.g = 0.5019607843137255f;
      clearColor.b = 0.5647058823529412f;
      break;

    case Color::Snow:
      clearColor.r = 1.0f;
      clearColor.g = 0.9803921568627451f;
      clearColor.b = 0.9803921568627451f;
      break;

    case Color::Springgreen:
      clearColor.r = 0.0f;
      clearColor.g = 1.0f;
      clearColor.b = 0.4980392156862745f;
      break;

    case Color::Steelblue:
      clearColor.r = 0.27450980392156865f;
      clearColor.g = 0.5098039215686274f;
      clearColor.b = 0.7058823529411765f;
      break;

    case Color::Tan:
      clearColor.r = 0.8235294117647058f;
      clearColor.g = 0.7058823529411765f;
      clearColor.b = 0.5490196078431373f;
      break;

    case Color::Thistle:
      clearColor.r = 0.8470588235294118f;
      clearColor.g = 0.7490196078431373f;
      clearColor.b = 0.8470588235294118f;
      break;

    case Color::Tomato:
      clearColor.r = 1.0f;
      clearColor.g = 0.38823529411764707f;
      clearColor.b = 0.2784313725490196f;
      break;

    case Color::Turquoise:
      clearColor.r = 0.25098039215686274f;
      clearColor.g = 0.8784313725490196f;
      clearColor.b = 0.8156862745098039f;
      break;

    case Color::Violet:
      clearColor.r = 0.9333333333333333f;
      clearColor.g = 0.5098039215686274f;
      clearColor.b = 0.9333333333333333f;
      break;

    case Color::Wheat:
      clearColor.r = 0.9607843137254902f;
      clearColor.g = 0.8705882352941177f;
      clearColor.b = 0.7019607843137254f;
      break;

    case Color::Whitesmoke:
      clearColor.r = 0.9607843137254902f;
      clearColor.g = 0.9607843137254902f;
      clearColor.b = 0.9607843137254902f;
      break;

    case Color::Yellowgreen:
      clearColor.r = 0.6039215686274509f;
      clearColor.g = 0.803921568627451f;
      clearColor.b = 0.19607843137254902f;
      break;
    }
}
