/**
 * @file BubbleMenuGeometry.h
 */

#ifndef BUBBLE_MENU_GEOMETRY_H
#define BUBBLE_MENU_GEOMETRY_H

#include <cvrMenu/MenuItem.h>
#include <cvrKernel/InteractionManager.h>

#include <osg/Geode>
#include <osg/MatrixTransform>
#include <osg/Texture2D>
#include <osg/Vec4>
#include <osgText/Text>
#include <osgText/Text3D>
#include <osg/ShapeDrawable>

#include <string>

namespace cvr
{

class BubbleMenu;

/**
 * @brief Base class of geometry implementation for BubbleMenu system
 */
class BubbleMenuGeometry
{
        friend class BubbleMenu;
    public:
        BubbleMenuGeometry();
        virtual ~BubbleMenuGeometry();

        /**
         * @brief Called when the pointer enters of exits the menu item
         */
        virtual void selectItem(bool on) = 0;

        /**
         * @brief Called once, used to create needed geometry
         */
        virtual void createGeometry(MenuItem * item) = 0;

        /**
         * @brief Called each time the MenuItem is flaged as dirty
         */
        virtual void updateGeometry() {}

        /**
         * @brief Process click and drag events
         */
        virtual void processEvent(InteractionEvent * event) = 0;

        /**
         * @brief Update the item intersection geometry with the given width
         */
        virtual void resetIntersect(float width);

        /**
         * @brief Called each frame the item is selected
         * @param pointerStart start point of pointer intersection test
         * @param pointerEnd end point of pointer intersection test
         */
        virtual void update(osg::Vec3 & pointerStart, osg::Vec3 & pointerEnd) {}

        /**
         * @brief Get MenuItem for this geometry
         */
        MenuItem * getMenuItem();

        /**
         * @brief Get the Geode that holds the intersection geometry
         */
        osg::Geode * getIntersect();

        /**
         * @brief Get the width of the geometry
         */
        float getWidth();

        /**
         * @brief Get the height of the geometry
         */
        float getHeight();

        /**
         * @brief Get the root node for the geometry
         */
        osg::MatrixTransform * getNode();

        virtual void showHoverText() {}
        virtual void hideHoverText() {}

    protected:
        static osg::Geometry * makeQuad(float width, float height, osg::Vec4 color,
                                 osg::Vec3 pos = osg::Vec3(0,0,0));
        static osg::Geometry * makeLine(osg::Vec3 p1, osg::Vec3 p2, osg::Vec4 color);

        static osg::Geometry * makeSphere(osg::Vec3 point, float radius, osg::Vec4 color);

        osg::Texture2D * loadIcon(std::string name);
        osgText::Text * makeText(std::string text, float size, osg::Vec3 pos, osg::Vec4 color, 
            osgText::Text::AlignmentType align = osgText::Text::CENTER_CENTER);
        static void calibrateTextSize(float textSize);
        osgText::Text3D * make3DText(std::string text, float size, osg::Vec3 pos, osg::Vec4 color, 
            osgText::Text::AlignmentType align = osgText::Text::CENTER_CENTER);

        float _width;
        float _height;
        osg::ref_ptr<osg::Geode> _intersect;
        osg::ref_ptr<osg::MatrixTransform> _node;
        MenuItem * _item;

        static std::string _iconDir;
        static osg::Vec4 _textColor;
        static osg::Vec4 _wireframeColor;
        static osg::Vec4 _textColorSelected;
        static osg::Vec4 _backgroundColor;
        static float _border;
        static float _iconHeight;
        static float _textSize;
        static float _radius;
        static int _tessellations;
        static osg::Vec4 _sphereColor;
        static osg::ref_ptr<osgText::Font> _font;
        static std::map<std::string,osg::ref_ptr<osg::Texture2D> > _iconCache;
};

BubbleMenuGeometry * createBubbleMenuGeometry(MenuItem * item, bool head = false);

}

#endif
