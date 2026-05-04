#include "ComplexPlane.h"
#include <cmath>
#include <sstream>
#include <iomanip>

using namespace sf;
using namespace std;

// constructor
ComplexPlane::ComplexPlane(int pixelWidth, int pixelHeight)
{
    m_pixel_size = {pixelWidth, pixelHeight};
    m_aspectRatio = static_cast<float>(pixelHeight) / static_cast<float>(pixelWidth);

    m_plane_center = {0, 0};
    m_plane_size = {BASE_WIDTH, BASE_HEIGHT * m_aspectRatio};
    m_zoomCount = 0;
    m_State = State::CALCULATING;

    // VertexArray
    m_vArray.setPrimitiveType(Points);
    m_vArray.resize(pixelWidth * pixelHeight);
}


// draw
void ComplexPlane::draw(RenderTarget& target, RenderStates states) const
{
    target.draw(m_vArray);
}


// updateRender
void ComplexPlane::updateRender()
{
    if (m_State == State::CALCULATING)
    {
        int pixelWidth = m_pixel_size.x;
        int pixelHeight = m_pixel_size.y;

        // x
        for (int i = 0; i < pixelHeight; ++i)
        {
            // y
            for (int j = 0; j < pixelWidth; ++j)
            {
                // Set the position in the array of screen coordinates
                m_vArray[j + i * pixelWidth].position = {static_cast<float>(j), static_cast<float>(i)};

                // Screen pixel location
                Vector2f coordinate = mapPixelToCoords({ j, i });

                //count iterations
                size_t iterations = countIterations(coordinate);

                // RGB
                Uint8 r, g, b;
                iterationsToRGB(iterations, r, g, b);

                // set the color
                m_vArray[j + i * pixelWidth].color = { r, g, b };
            }
        }
        m_State = State::DISPLAYING;
    }
}

// zoomIn
void ComplexPlane::zoomIn()
{
    m_zoomCount++;
    float xSize = BASE_WIDTH * pow(BASE_ZOOM, m_zoomCount);
    float ySize = BASE_HEIGHT * m_aspectRatio * pow(BASE_ZOOM, m_zoomCount);
    m_plane_size = {xSize, ySize};
    m_State = State::CALCULATING;
}

// zoomOut
void ComplexPlane::zoomOut()
{
    m_zoomCount--;
    float xSize = BASE_WIDTH * pow(BASE_ZOOM, m_zoomCount);
    float ySize = BASE_HEIGHT * m_aspectRatio * pow(BASE_ZOOM, m_zoomCount);
    m_plane_size = {xSize, ySize};
    m_State = State::CALCULATING;
}


// setCenter
void ComplexPlane::setCenter(Vector2i mousePixel)
{
    m_plane_center = mapPixelToCoords(mousePixel);
    m_State = State::CALCULATING;
}


// setMouseLocation
void ComplexPlane::setMouseLocation(Vector2i mousePixel)
{
    m_mouseLocation = mapPixelToCoords(mousePixel);
}

// loadText
void ComplexPlane::loadText(Text& text)
{
    ostringstream oss;
    oss << fixed << setprecision(6);
    oss << "Mandelbrot Set\n";
    oss << "Center: (" << m_plane_center.x << ", " << m_plane_center.y << ")\n";
    oss << "Cursor: (" << m_mouseLocation.x << ", " << m_mouseLocation.y << ")\n";
    oss << "Left-click to zoom in \nRight-click to zoom out\n";
    text.setString(oss.str());
}

// countIterations
int ComplexPlane::countIterations(Vector2f coord)
{
    complex<double> c(coord.x, coord.y);
    complex<double> z(0, 0);
    int i = 0;
    while (abs(z) < 2.0 && i < MAX_ITER)
    {
        z = z * z + c;
        ++i;
    }
    return i;
}


// iterationsToRGB. 6 colors (5 + black inside)
void ComplexPlane::iterationsToRGB(size_t count, Uint8& r, Uint8& g, Uint8& b)
{
    // black inside
    if (count == MAX_ITER)
    {
        r = g = b = 0;   
        return;
    }

    // Between [0 : MAX_ITER) ther are 5 regions for 5 colors
    size_t bandSize = MAX_ITER / 5;
    size_t band = count / bandSize;
    size_t offset = count % bandSize;   // gradient

    // Scale offset
    Uint8 transition = static_cast<Uint8>((offset * 255) / bandSize);

    switch (band)
    {
        // starts with low iterations
    case 0:  
        r = static_cast<Uint8>(128 - transition / 2); // Purple
        g = 0;
        b = static_cast<Uint8>(128 + transition / 2); // blue
        break;
    case 1:  
        r = 0;
        g = transition;
        b = static_cast<Uint8>(255 - transition); // Turquoise
        break;
    case 2:  // to Green
        r = 0;
        g = static_cast<Uint8>(200 + transition / 5);
        b = static_cast<Uint8>(200 - transition);
        break;
    case 3:  // to Yellow
        r = transition;
        g = static_cast<Uint8>(255 - transition / 4);
        b = 0;
        break;
        //high iterations
    default: // Red  
        r = 255;
        g = static_cast<Uint8>(255 - transition);
        b = 0;
        break;
    }
}


// mapPixelToCoords. ((n - a) / (b - a)) * (d - c) + c
Vector2f ComplexPlane::mapPixelToCoords(Vector2i mousePixel)
{
    float realMin = m_plane_center.x - m_plane_size.x / 2;
    float realMax = m_plane_center.x + m_plane_size.x / 2;
    float imaginaryMin = m_plane_center.y - m_plane_size.y / 2;
    float imaginaryMax = m_plane_center.y + m_plane_size.y / 2;

    // x pixel range x => [0, 1920]
    float real = ((static_cast<float>(mousePixel.x) - 0) / (static_cast<float>(m_pixel_size.x) - 0)) 
        * (realMax - realMin) + realMin;

    // y pixel range y => [1080, 0]
    float imaginary = ((static_cast<float>(mousePixel.y) - static_cast<float>(m_pixel_size.y)) / 
        (0 - static_cast<float>(m_pixel_size.y))) * (imaginaryMax - imaginaryMin) + imaginaryMin;

    return {real, imaginary};
}