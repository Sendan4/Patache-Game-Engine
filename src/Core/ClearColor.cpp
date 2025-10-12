#include <vulkan/vulkan.hpp>
#include "PatacheEngine/VmaUsage.hpp"
#include <SDL3/SDL.h>

#include "PatacheEngine/PatacheEngine.hpp"

void
Patache::Engine::ClearColorRGBA (const float & rR, const float & rG, const float & rB,
                                 const float & rA)
{
  clearColor.r = rR;
  clearColor.g = rG;
  clearColor.b = rB;
  clearColor.a = rA;
}

void
Patache::Engine::ClearColorRGBA (const float & rR, const float & rG, const float & rB)
{
  clearColor.r = rR;
  clearColor.g = rG;
  clearColor.b = rB;
}

void
Patache::Engine::ClearColor (const Color & rColor)
{
  switch (rColor)
    {
    // Basic Colors
    case Patache::Color::eBlack:
      clearColor.r = 0.0f;
      clearColor.g = 0.0f;
      clearColor.b = 0.0f;
      break;

    case Patache::Color::eSilver:
      clearColor.r = 0.7529411764705882f;
      clearColor.g = 0.7529411764705882f;
      clearColor.b = 0.7529411764705882f;
      break;

    case Patache::Color::eGray:
      clearColor.r = 0.5019607843137255f;
      clearColor.g = 0.5019607843137255f;
      clearColor.b = 0.5019607843137255f;
      break;

    case Patache::Color::eWhite:
      clearColor.r = 1.0f;
      clearColor.g = 1.0f;
      clearColor.b = 1.0f;
      break;

    case Patache::Color::eMaroon:
      clearColor.r = 0.5019607843137255f;
      clearColor.g = 0.0f;
      clearColor.b = 0.0f;
      break;

    case Patache::Color::eRed:
      clearColor.r = 1.0f;
      clearColor.g = 0.0f;
      clearColor.b = 0.0f;
      break;

    case Patache::Color::ePurple:
      clearColor.r = 0.5019607843137255f;
      clearColor.g = 0.0f;
      clearColor.b = 0.5019607843137255f;
      break;

    case Patache::Color::eFuchsia:
      clearColor.r = 1.0f;
      clearColor.g = 0.0f;
      clearColor.b = 1.0f;
      break;

    case Patache::Color::eGreen:
      clearColor.r = 0.0f;
      clearColor.g = 0.5019607843137255f;
      clearColor.b = 0.0f;
      break;

    case Patache::Color::eLime:
      clearColor.r = 0.0f;
      clearColor.g = 1.0f;
      clearColor.b = 0.0f;
      break;

    case Patache::Color::eOlive:
      clearColor.r = 0.5019607843137255f;
      clearColor.g = 0.5019607843137255f;
      clearColor.b = 0.0f;
      break;

    case Patache::Color::eYellow:
      clearColor.r = 1.0f;
      clearColor.g = 1.0f;
      clearColor.b = 0.0f;
      break;

    case Patache::Color::eNavy:
      clearColor.r = 0.0f;
      clearColor.g = 0.0f;
      clearColor.b = 0.5019607843137255f;
      break;

    case Patache::Color::eBlue:
      clearColor.r = 0.0f;
      clearColor.g = 0.0f;
      clearColor.b = 1.0f;
      break;

    case Patache::Color::eTeal:
      clearColor.r = 0.0f;
      clearColor.g = 0.5019607843137255f;
      clearColor.b = 0.5019607843137255f;
      break;

    case Patache::Color::eAqua:
      clearColor.r = 0.0f;
      clearColor.g = 1.0f;
      clearColor.b = 1.0f;
      break;

    // Extended colors
    case Patache::Color::eAliceblue:
      clearColor.r = 0.9411764705882353f;
      clearColor.g = 0.9725490196078431f;
      clearColor.b = 1.0f;
      break;

    case Patache::Color::eAntiquewhite:
      clearColor.r = 0.9803921568627451f;
      clearColor.g = 0.9215686274509803f;
      clearColor.b = 0.8431372549019608f;
      break;

    case Patache::Color::eAquamarine:
      clearColor.r = 0.4980392156862745f;
      clearColor.g = 1.0f;
      clearColor.b = 0.8313725490196079f;
      break;

    case Patache::Color::eAzure:
      clearColor.r = 0.9411764705882353f;
      clearColor.g = 1.0f;
      clearColor.b = 1.0f;
      break;

    case Patache::Color::eBeige:
      clearColor.r = 0.9607843137254902f;
      clearColor.g = 0.9607843137254902f;
      clearColor.b = 0.8627450980392157f;
      break;

    case Patache::Color::eBisque:
      clearColor.r = 1.0f;
      clearColor.g = 0.8941176470588236f;
      clearColor.b = 0.7686274509803922f;
      break;

    case Patache::Color::eBlanchedalmond:
      clearColor.r = 1.0f;
      clearColor.g = 0.9215686274509803f;
      clearColor.b = 0.803921568627451f;
      break;

    case Patache::Color::eBlueviolet:
      clearColor.r = 0.5411764705882353f;
      clearColor.g = 0.16862745098039217f;
      clearColor.b = 0.8862745098039215f;
      break;

    case Patache::Color::eBrown:
      clearColor.r = 0.6470588235294118f;
      clearColor.g = 0.16470588235294117f;
      clearColor.b = 0.16470588235294117f;
      break;

    case Patache::Color::eBurlywood:
      clearColor.r = 0.8705882352941177f;
      clearColor.g = 0.7215686274509804f;
      clearColor.b = 0.5294117647058824f;
      break;

    case Patache::Color::eCadetblue:
      clearColor.r = 0.37254901960784315f;
      clearColor.g = 0.6196078431372549f;
      clearColor.b = 0.6274509803921569f;
      break;

    case Patache::Color::eChartreuse:
      clearColor.r = 0.4980392156862745f;
      clearColor.g = 1.0f;
      clearColor.b = 0.0f;
      break;

    case Patache::Color::eChocolate:
      clearColor.r = 0.8235294117647058f;
      clearColor.g = 0.4117647058823529f;
      clearColor.b = 0.11764705882352941f;
      break;

    case Patache::Color::eCoral:
      clearColor.r = 1.0f;
      clearColor.g = 0.4980392156862745f;
      clearColor.b = 0.3137254901960784f;
      break;

    case Patache::Color::eCornflowerblue:
      clearColor.r = 0.39215686274509803f;
      clearColor.g = 0.5843137254901961f;
      clearColor.b = 0.9294117647058824f;
      break;

    case Patache::Color::eCornsilk:
      clearColor.r = 1.0f;
      clearColor.g = 0.9725490196078431f;
      clearColor.b = 0.8627450980392157f;
      break;

    case Patache::Color::eCrimson:
      clearColor.r = 0.8627450980392157f;
      clearColor.g = 0.0784313725490196f;
      clearColor.b = 0.23529411764705882f;
      break;

    case Patache::Color::eCyan:
      clearColor.r = 0.0f;
      clearColor.g = 1.0f;
      clearColor.b = 1.0f;
      break;

    case Patache::Color::eDarkblue:
      clearColor.r = 0.0f;
      clearColor.g = 0.0f;
      clearColor.b = 0.5450980392156862f;
      break;

    case Patache::Color::eDarkcyan:
      clearColor.r = 0.0f;
      clearColor.g = 0.5450980392156862f;
      clearColor.b = 0.5450980392156862f;
      break;

    case Patache::Color::eDarkgoldenrod:
      clearColor.r = 0.7215686274509804f;
      clearColor.g = 0.5254901960784314f;
      clearColor.b = 0.043137254901960784f;
      break;

    case Patache::Color::eDarkgray:
      clearColor.r = 0.6627450980392157f;
      clearColor.g = 0.6627450980392157f;
      clearColor.b = 0.6627450980392157f;
      break;

    case Patache::Color::eDarkgreen:
      clearColor.r = 0.0f;
      clearColor.g = 0.39215686274509803f;
      clearColor.b = 0.0f;
      break;

    case Patache::Color::eDarkkhaki:
      clearColor.r = 0.7411764705882353f;
      clearColor.g = 0.7176470588235294f;
      clearColor.b = 0.4196078431372549f;
      break;

    case Patache::Color::eDarkmagenta:
      clearColor.r = 0.5450980392156862f;
      clearColor.g = 0.0f;
      clearColor.b = 0.5450980392156862f;
      break;

    case Patache::Color::eDarkolivegreen:
      clearColor.r = 0.3333333333333333f;
      clearColor.g = 0.4196078431372549f;
      clearColor.b = 0.1843137254901961f;
      break;

    case Patache::Color::eDarkorange:
      clearColor.r = 1.0f;
      clearColor.g = 0.5490196078431373f;
      clearColor.b = 0.0f;
      break;

    case Patache::Color::eDarkorchid:
      clearColor.r = 0.6f;
      clearColor.g = 0.19607843137254902f;
      clearColor.b = 0.8f;
      break;

    case Patache::Color::eDarkred:
      clearColor.r = 0.5450980392156862f;
      clearColor.g = 0.0f;
      clearColor.b = 0.0f;
      break;

    case Patache::Color::eDarksalmon:
      clearColor.r = 0.9137254901960784f;
      clearColor.g = 0.5882352941176471f;
      clearColor.b = 0.47843137254901963f;
      break;

    case Patache::Color::eDarkseagreen:
      clearColor.r = 0.5607843137254902f;
      clearColor.g = 0.7372549019607844f;
      clearColor.b = 0.5607843137254902f;
      break;

    case Patache::Color::eDarkslateblue:
      clearColor.r = 0.2823529411764706f;
      clearColor.g = 0.23921568627450981f;
      clearColor.b = 0.5450980392156862f;
      break;

    case Patache::Color::eDarkslategray:
      clearColor.r = 0.1843137254901961f;
      clearColor.g = 0.30980392156862746f;
      clearColor.b = 0.30980392156862746f;
      break;

    case Patache::Color::eDarkturquoise:
      clearColor.r = 0.0f;
      clearColor.g = 0.807843137254902f;
      clearColor.b = 0.8196078431372549f;
      break;

    case Patache::Color::eDarkviolet:
      clearColor.r = 0.5803921568627451f;
      clearColor.g = 0.0f;
      clearColor.b = 0.8274509803921568f;
      break;

    case Patache::Color::eDeeppink:
      clearColor.r = 1.0f;
      clearColor.g = 0.0784313725490196f;
      clearColor.b = 0.5764705882352941f;
      break;

    case Patache::Color::eDeepskyblue:
      clearColor.r = 0.0f;
      clearColor.g = 0.7490196078431373f;
      clearColor.b = 1.0f;
      break;

    case Patache::Color::eDimgray:
      clearColor.r = 0.4117647058823529f;
      clearColor.g = 0.4117647058823529f;
      clearColor.b = 0.4117647058823529f;
      break;

    case Patache::Color::eDodgerblue:
      clearColor.r = 0.11764705882352941f;
      clearColor.g = 0.5647058823529412f;
      clearColor.b = 1.0f;
      break;

    case Patache::Color::eFirebrick:
      clearColor.r = 0.6980392156862745f;
      clearColor.g = 0.13333333333333333f;
      clearColor.b = 0.13333333333333333f;
      break;

    case Patache::Color::eFloralwhite:
      clearColor.r = 1.0f;
      clearColor.g = 0.9803921568627451f;
      clearColor.b = 0.9411764705882353f;
      break;

    case Patache::Color::eForestgreen:
      clearColor.r = 0.13333333333333333f;
      clearColor.g = 0.5450980392156862f;
      clearColor.b = 0.13333333333333333f;
      break;

    case Patache::Color::eGainsboro:
      clearColor.r = 0.8627450980392157f;
      clearColor.g = 0.8627450980392157f;
      clearColor.b = 0.8627450980392157f;
      break;

    case Patache::Color::eGhostwhite:
      clearColor.r = 0.9725490196078431f;
      clearColor.g = 0.9725490196078431f;
      clearColor.b = 1.0f;
      break;

    case Patache::Color::eGold:
      clearColor.r = 1.0f;
      clearColor.g = 0.8431372549019608f;
      clearColor.b = 0.0f;
      break;

    case Patache::Color::eGoldenrod:
      clearColor.r = 0.8549019607843137f;
      clearColor.g = 0.6470588235294118f;
      clearColor.b = 0.12549019607843137f;
      break;

    case Patache::Color::eGreenyellow:
      clearColor.r = 0.6784313725490196f;
      clearColor.g = 1.0f;
      clearColor.b = 0.1843137254901961f;
      break;

    case Patache::Color::eHoneydew:
      clearColor.r = 0.9411764705882353f;
      clearColor.g = 1.0f;
      clearColor.b = 0.9411764705882353f;
      break;

    case Patache::Color::eHotpink:
      clearColor.r = 1.0f;
      clearColor.g = 0.4117647058823529f;
      clearColor.b = 0.7058823529411765f;
      break;

    case Patache::Color::eIndianred:
      clearColor.r = 0.803921568627451f;
      clearColor.g = 0.3607843137254902f;
      clearColor.b = 0.3607843137254902f;
      break;

    case Patache::Color::eIndigo:
      clearColor.r = 0.29411764705882354f;
      clearColor.g = 0.0f;
      clearColor.b = 0.5098039215686274f;
      break;

    case Patache::Color::eIvory:
      clearColor.r = 1.0f;
      clearColor.g = 1.0f;
      clearColor.b = 0.9411764705882353f;
      break;

    case Patache::Color::eKhaki:
      clearColor.r = 0.9411764705882353f;
      clearColor.g = 0.9019607843137255f;
      clearColor.b = 0.5490196078431373f;
      break;

    case Patache::Color::eLavender:
      clearColor.r = 0.9019607843137255f;
      clearColor.g = 0.9019607843137255f;
      clearColor.b = 0.9803921568627451f;
      break;

    case Patache::Color::eLavenderblush:
      clearColor.r = 1.0f;
      clearColor.g = 0.9411764705882353f;
      clearColor.b = 0.9607843137254902f;
      break;

    case Patache::Color::eLawngreen:
      clearColor.r = 0.48627450980392156f;
      clearColor.g = 0.9882352941176471f;
      clearColor.b = 0.0f;
      break;

    case Patache::Color::eLemonchiffon:
      clearColor.r = 1.0f;
      clearColor.g = 0.9803921568627451f;
      clearColor.b = 0.803921568627451f;
      break;

    case Patache::Color::eLightblue:
      clearColor.r = 0.6784313725490196f;
      clearColor.g = 0.8470588235294118f;
      clearColor.b = 0.9019607843137255f;
      break;

    case Patache::Color::eLightcoral:
      clearColor.r = 0.9411764705882353f;
      clearColor.g = 0.5019607843137255f;
      clearColor.b = 0.5019607843137255f;
      break;

    case Patache::Color::eLightcyan:
      clearColor.r = 0.8784313725490196f;
      clearColor.g = 1.0f;
      clearColor.b = 1.0f;
      break;

    case Patache::Color::eLightgoldenrodyellow:
      clearColor.r = 0.9803921568627451f;
      clearColor.g = 0.9803921568627451f;
      clearColor.b = 0.8235294117647058f;
      break;

    case Patache::Color::eLightgray:
      clearColor.r = 0.8274509803921568f;
      clearColor.g = 0.8274509803921568f;
      clearColor.b = 0.8274509803921568f;
      break;

    case Patache::Color::eLightgreen:
      clearColor.r = 0.5647058823529412f;
      clearColor.g = 0.9333333333333333f;
      clearColor.b = 0.5647058823529412f;
      break;

    case Patache::Color::eLightpink:
      clearColor.r = 1.0f;
      clearColor.g = 0.7137254901960784f;
      clearColor.b = 0.7568627450980392f;
      break;

    case Patache::Color::eLightsalmon:
      clearColor.r = 1.0f;
      clearColor.g = 0.6274509803921569f;
      clearColor.b = 0.47843137254901963f;
      break;

    case Patache::Color::eLightseagreen:
      clearColor.r = 0.12549019607843137f;
      clearColor.g = 0.6980392156862745f;
      clearColor.b = 0.6666666666666666f;
      break;

    case Patache::Color::eLightskyblue:
      clearColor.r = 0.5294117647058824f;
      clearColor.g = 0.807843137254902f;
      clearColor.b = 0.9803921568627451f;
      break;

    case Patache::Color::eLightslategray:
      clearColor.r = 0.4666666666666667f;
      clearColor.g = 0.5333333333333333f;
      clearColor.b = 0.6f;
      break;

    case Patache::Color::eLightsteelblue:
      clearColor.r = 0.6901960784313725f;
      clearColor.g = 0.7686274509803922f;
      clearColor.b = 0.8705882352941177f;
      break;

    case Patache::Color::eLightyellow:
      clearColor.r = 1.0f;
      clearColor.g = 1.0f;
      clearColor.b = 0.8784313725490196f;
      break;

    case Patache::Color::eLimegreen:
      clearColor.r = 0.19607843137254902f;
      clearColor.g = 0.803921568627451f;
      clearColor.b = 0.19607843137254902f;
      break;

    case Patache::Color::eLinen:
      clearColor.r = 0.9803921568627451f;
      clearColor.g = 0.9411764705882353f;
      clearColor.b = 0.9019607843137255f;
      break;

    case Patache::Color::eMagenta:
      clearColor.r = 1.0f;
      clearColor.g = 0.0f;
      clearColor.b = 1.0f;
      break;

    case Patache::Color::eMediumaquamarine:
      clearColor.r = 0.4f;
      clearColor.g = 0.803921568627451f;
      clearColor.b = 0.6666666666666666f;
      break;

    case Patache::Color::eMediumblue:
      clearColor.r = 0.0f;
      clearColor.g = 0.0f;
      clearColor.b = 0.803921568627451f;
      break;

    case Patache::Color::eMediumorchid:
      clearColor.r = 0.7294117647058823f;
      clearColor.g = 0.3333333333333333f;
      clearColor.b = 0.8274509803921568f;
      break;

    case Patache::Color::eMediumpurple:
      clearColor.r = 0.5764705882352941f;
      clearColor.g = 0.4392156862745098f;
      clearColor.b = 0.8588235294117647f;
      break;

    case Patache::Color::eMediumseagreen:
      clearColor.r = 0.23529411764705882f;
      clearColor.g = 0.7019607843137254f;
      clearColor.b = 0.44313725490196076f;
      break;

    case Patache::Color::eMediumslateblue:
      clearColor.r = 0.4823529411764706f;
      clearColor.g = 0.40784313725490196f;
      clearColor.b = 0.9333333333333333f;
      break;

    case Patache::Color::eMediumspringgreen:
      clearColor.r = 0.0f;
      clearColor.g = 0.9803921568627451f;
      clearColor.b = 0.6039215686274509f;
      break;

    case Patache::Color::eMediumturquoise:
      clearColor.r = 0.2823529411764706f;
      clearColor.g = 0.8196078431372549f;
      clearColor.b = 0.8f;
      break;

    case Patache::Color::eMediumvioletred:
      clearColor.r = 0.7803921568627451f;
      clearColor.g = 0.08235294117647059f;
      clearColor.b = 0.5215686274509804f;
      break;

    case Patache::Color::eMidnightblue:
      clearColor.r = 0.09803921568627451f;
      clearColor.g = 0.09803921568627451f;
      clearColor.b = 0.4392156862745098f;
      break;

    case Patache::Color::eMintcream:
      clearColor.r = 0.9607843137254902f;
      clearColor.g = 1.0f;
      clearColor.b = 0.9803921568627451f;
      break;

    case Patache::Color::eMistyrose:
      clearColor.r = 1.0f;
      clearColor.g = 0.8941176470588236f;
      clearColor.b = 0.8823529411764706f;
      break;

    case Patache::Color::eMoccasin:
      clearColor.r = 1.0f;
      clearColor.g = 0.8941176470588236f;
      clearColor.b = 0.7098039215686275f;
      break;

    case Patache::Color::eNavajowhite:
      clearColor.r = 1.0f;
      clearColor.g = 0.8705882352941177f;
      clearColor.b = 0.6784313725490196f;
      break;

    case Patache::Color::eOldlace:
      clearColor.r = 0.9921568627450981f;
      clearColor.g = 0.9607843137254902f;
      clearColor.b = 0.9019607843137255f;
      break;

    case Patache::Color::eOlivedrab:
      clearColor.r = 0.4196078431372549f;
      clearColor.g = 0.5568627450980392f;
      clearColor.b = 0.13725490196078433f;
      break;

    case Patache::Color::eOrange:
      clearColor.r = 1.0f;
      clearColor.g = 0.6470588235294118f;
      clearColor.b = 0.0f;
      break;

    case Patache::Color::eOrangered:
      clearColor.r = 1.0f;
      clearColor.g = 0.27058823529411763f;
      clearColor.b = 0.0f;
      break;

    case Patache::Color::eOrchid:
      clearColor.r = 0.8549019607843137f;
      clearColor.g = 0.4392156862745098f;
      clearColor.b = 0.8392156862745098f;
      break;

    case Patache::Color::ePalegoldenrod:
      clearColor.r = 0.9333333333333333f;
      clearColor.g = 0.9098039215686274f;
      clearColor.b = 0.6666666666666666f;
      break;

    case Patache::Color::ePalegreen:
      clearColor.r = 0.596078431372549f;
      clearColor.g = 0.984313725490196f;
      clearColor.b = 0.596078431372549f;
      break;

    case Patache::Color::ePaleturquoise:
      clearColor.r = 0.6862745098039216f;
      clearColor.g = 0.9333333333333333f;
      clearColor.b = 0.9333333333333333f;
      break;

    case Patache::Color::ePalevioletred:
      clearColor.r = 0.8588235294117647f;
      clearColor.g = 0.4392156862745098f;
      clearColor.b = 0.5764705882352941f;
      break;

    case Patache::Color::ePapayawhip:
      clearColor.r = 1.0f;
      clearColor.g = 0.9372549019607843f;
      clearColor.b = 0.8352941176470589f;
      break;

    case Patache::Color::ePeachpuff:
      clearColor.r = 1.0f;
      clearColor.g = 0.8549019607843137f;
      clearColor.b = 0.7254901960784313f;
      break;

    case Patache::Color::ePeru:
      clearColor.r = 0.803921568627451f;
      clearColor.g = 0.5215686274509804f;
      clearColor.b = 0.24705882352941178f;
      break;

    case Patache::Color::ePink:
      clearColor.r = 1.0f;
      clearColor.g = 0.7529411764705882f;
      clearColor.b = 0.796078431372549f;
      break;

    case Patache::Color::ePlum:
      clearColor.r = 0.8666666666666667f;
      clearColor.g = 0.6274509803921569f;
      clearColor.b = 0.8666666666666667f;
      break;

    case Patache::Color::ePowderblue:
      clearColor.r = 0.6901960784313725f;
      clearColor.g = 0.8784313725490196f;
      clearColor.b = 0.9019607843137255f;
      break;

    case Patache::Color::eRosybrown:
      clearColor.r = 0.7372549019607844f;
      clearColor.g = 0.5607843137254902f;
      clearColor.b = 0.5607843137254902f;
      break;

    case Patache::Color::eRoyalblue:
      clearColor.r = 0.2549019607843137f;
      clearColor.g = 0.4117647058823529f;
      clearColor.b = 0.8823529411764706f;
      break;

    case Patache::Color::eSaddlebrown:
      clearColor.r = 0.5450980392156862f;
      clearColor.g = 0.27058823529411763f;
      clearColor.b = 0.07450980392156863f;
      break;

    case Patache::Color::eSalmon:
      clearColor.r = 0.9803921568627451f;
      clearColor.g = 0.5019607843137255f;
      clearColor.b = 0.4470588235294118f;
      break;

    case Patache::Color::eSandybrown:
      clearColor.r = 0.9568627450980393f;
      clearColor.g = 0.6431372549019608f;
      clearColor.b = 0.3764705882352941f;
      break;

    case Patache::Color::eSeagreen:
      clearColor.r = 0.1803921568627451f;
      clearColor.g = 0.5450980392156862f;
      clearColor.b = 0.3411764705882353f;
      break;

    case Patache::Color::eSeashell:
      clearColor.r = 1.0f;
      clearColor.g = 0.9607843137254902f;
      clearColor.b = 0.9333333333333333f;
      break;

    case Patache::Color::eSienna:
      clearColor.r = 0.6274509803921569f;
      clearColor.g = 0.3215686274509804f;
      clearColor.b = 0.17647058823529413f;
      break;

    case Patache::Color::eSkyblue:
      clearColor.r = 0.5294117647058824f;
      clearColor.g = 0.807843137254902f;
      clearColor.b = 0.9215686274509803f;
      break;

    case Patache::Color::eSlateblue:
      clearColor.r = 0.41568627450980394f;
      clearColor.g = 0.35294117647058826f;
      clearColor.b = 0.803921568627451f;
      break;

    case Patache::Color::eSlategray:
      clearColor.r = 0.4392156862745098f;
      clearColor.g = 0.5019607843137255f;
      clearColor.b = 0.5647058823529412f;
      break;

    case Patache::Color::eSnow:
      clearColor.r = 1.0f;
      clearColor.g = 0.9803921568627451f;
      clearColor.b = 0.9803921568627451f;
      break;

    case Patache::Color::eSpringgreen:
      clearColor.r = 0.0f;
      clearColor.g = 1.0f;
      clearColor.b = 0.4980392156862745f;
      break;

    case Patache::Color::eSteelblue:
      clearColor.r = 0.27450980392156865f;
      clearColor.g = 0.5098039215686274f;
      clearColor.b = 0.7058823529411765f;
      break;

    case Patache::Color::eTan:
      clearColor.r = 0.8235294117647058f;
      clearColor.g = 0.7058823529411765f;
      clearColor.b = 0.5490196078431373f;
      break;

    case Patache::Color::eThistle:
      clearColor.r = 0.8470588235294118f;
      clearColor.g = 0.7490196078431373f;
      clearColor.b = 0.8470588235294118f;
      break;

    case Patache::Color::eTomato:
      clearColor.r = 1.0f;
      clearColor.g = 0.38823529411764707f;
      clearColor.b = 0.2784313725490196f;
      break;

    case Patache::Color::eTurquoise:
      clearColor.r = 0.25098039215686274f;
      clearColor.g = 0.8784313725490196f;
      clearColor.b = 0.8156862745098039f;
      break;

    case Patache::Color::eViolet:
      clearColor.r = 0.9333333333333333f;
      clearColor.g = 0.5098039215686274f;
      clearColor.b = 0.9333333333333333f;
      break;

    case Patache::Color::eWheat:
      clearColor.r = 0.9607843137254902f;
      clearColor.g = 0.8705882352941177f;
      clearColor.b = 0.7019607843137254f;
      break;

    case Patache::Color::eWhitesmoke:
      clearColor.r = 0.9607843137254902f;
      clearColor.g = 0.9607843137254902f;
      clearColor.b = 0.9607843137254902f;
      break;

    case Patache::Color::eYellowgreen:
      clearColor.r = 0.6039215686274509f;
      clearColor.g = 0.803921568627451f;
      clearColor.b = 0.19607843137254902f;
      break;
    }
}
