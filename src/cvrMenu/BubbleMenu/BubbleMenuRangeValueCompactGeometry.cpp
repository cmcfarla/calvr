#include <cvrMenu/BubbleMenu/BubbleMenuRangeValueCompactGeometry.h>
#include <cvrMenu/MenuRangeValueCompact.h>
#include <cvrInput/TrackingManager.h>
#include <cvrKernel/InteractionManager.h>

#include <osg/Geometry>
#include <osg/PolygonMode>
#include <osg/LineWidth>

#include <cstdio>
#include <cmath>

#ifdef WIN32
#define snprintf _snprintf
#endif

using namespace cvr;

BubbleMenuRangeValueCompactGeometry::BubbleMenuRangeValueCompactGeometry() :
        BubbleMenuGeometry()
{

}

BubbleMenuRangeValueCompactGeometry::~BubbleMenuRangeValueCompactGeometry()
{

}

void BubbleMenuRangeValueCompactGeometry::selectItem(bool on)
{
    //std::cerr << "Select Item called " << on << std::endl;
    _node->removeChildren(0,_node->getNumChildren());
    if(on)
    {
        _node->addChild(_groupSelected);
	_width = _widthValue;
    }
    else
    {
        _node->addChild(_group);
	_width = _widthLabel;
    }
}

void BubbleMenuRangeValueCompactGeometry::createGeometry(MenuItem * item)
{
    _item = item;
    _node = new osg::MatrixTransform();
    _intersect = new osg::Geode();

    _group = new osg::Group();
    _groupSelected = new osg::Group();
    _geode = new osg::Geode();
    _geodeSelected = new osg::Geode();
    _geodeIcon = new osg::Geode();

    _group->addChild(_geode);
    _group->addChild(_geodeIcon);
    _group->addChild(_intersect);

    _groupSelected->addChild(_geodeSelected);
    _groupSelected->addChild(_geodeIcon);
    _groupSelected->addChild(_intersect);

    _node->addChild(_group);

    osg::Geometry * geo = makeQuad(_iconHeight,-_iconHeight,
            osg::Vec4(1.0,1.0,1.0,1.0),osg::Vec3(0,-2,0));
//    _geodeIcon->addDrawable(geo);

    //TODO create icon
    //_rvIcon = loadIcon("greater.rgb");

    if(_rvIcon)
    {
        osg::StateSet * stateset = _geodeIcon->getOrCreateStateSet();
        stateset->setTextureAttributeAndModes(0,_rvIcon,
                osg::StateAttribute::ON);
    }

    MenuRangeValueCompact * mrv = (MenuRangeValueCompact*)item;

    const char * printstr;
    if((mrv->getMin() >= 0 && mrv->getMax() >= 0)
            || (mrv->getMin() < 0 && mrv->getMax() < 0))
    {
        printstr = "%6f";
	_sign = true;
    }
    else
    {
        printstr = "% 6f";
	_sign = false;
    }

    _label = make3DText(mrv->getLabel(), _textSize, osg::Vec3(0,0,0),
        _textColor);

    osg::BoundingBox bbb = _label->getBound();
    float width = bbb.xMax() - bbb.xMin();

    _label->setPosition(osg::Vec3(_radius/2, 0, 0));
    _geode->addDrawable(_label.get());
    
    // Unselected sphere
    osg::Geometry * sphereGeom = makeSphere(osg::Vec3(_radius/2,0,0), _radius,
        osg::Vec4(0,1,0,1));
    osg::PolygonMode * polygonMode = new osg::PolygonMode();
    polygonMode->setMode(osg::PolygonMode::FRONT_AND_BACK, 
        osg::PolygonMode::LINE);
    sphereGeom->getOrCreateStateSet()->setAttribute(polygonMode, 
        osg::StateAttribute::ON);

    _geode->addDrawable(sphereGeom);
    
    // Selected sphere
    osg::Geometry * sphereSelectedGeom = makeSphere(osg::Vec3(_radius/2,0,0),
        _radius, osg::Vec4(0,1,0,1));
    sphereSelectedGeom->getOrCreateStateSet()->setAttribute(polygonMode, 
        osg::StateAttribute::ON);

    osg::LineWidth * lineWidth = new osg::LineWidth();
    lineWidth->setWidth(3);
    sphereSelectedGeom->getOrCreateStateSet()->setAttribute(lineWidth, 
        osg::StateAttribute::ON);
    _geodeSelected->addDrawable(sphereSelectedGeom);


    char buffer[7];
    _widthLabel = _widthValue = _iconHeight + _border;
    snprintf(buffer,7,printstr,mrv->getValue());

    _currentValue = make3DText(buffer,_textSize, osg::Vec3(_radius/2,0,0), 
        _textColorSelected);
    _geodeSelected->addDrawable(_currentValue.get());

    osg::BoundingBox bb;
    bb = _label->getBound();
    _widthLabel += bb.xMax() - bb.xMin();

    bb = _currentValue->getBound();
    _widthValue += bb.xMax() - bb.xMin();

    _height = _iconHeight;
    _width = _widthLabel;

    // Hover text
    _textGeode = new osg::Geode();
    osgText::Text * hoverTextNode = makeText(item->getHoverText(), _textSize,
//        osg::Vec3(0,0,_radius), osg::Vec4(1,1,1,1), osgText::Text::LEFT_CENTER);
       osg::Vec3(_radius/2,0,1.5*_radius), osg::Vec4(1,1,1,1), osgText::Text::CENTER_CENTER);

    osg::StateSet * ss = _textGeode->getOrCreateStateSet();
    ss->setMode(GL_BLEND, osg::StateAttribute::ON);
    ss->setMode(GL_DEPTH_TEST, osg::StateAttribute::OFF);
    ss->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
    ss->setRenderBinDetails(11, "Render Bin");

    hoverTextNode->setFont(_font);
    _textGeode->addDrawable(hoverTextNode);
}

void BubbleMenuRangeValueCompactGeometry::updateGeometry()
{
    _geodeSelected->removeDrawable(_currentValue.get());

    char buffer[7];
    const char * printstr;
    if(_sign)
    {
        printstr = "% 6f";
    }
    else
    {
        printstr = "%6f";
    }

    MenuRangeValueCompact * mrv = (MenuRangeValueCompact*)_item;

    snprintf(buffer,7,printstr,mrv->getValue());
    _currentValue = make3DText(buffer,_textSize,
            osg::Vec3(_radius/2,0,0),_textColor);

    _geodeSelected->addDrawable(_currentValue.get());

    osg::BoundingBox bb = _currentValue->getBound();
    _widthValue = bb.xMax() - bb.xMin() + _border + _iconHeight;

    if(_group->getNumParents())
    {
	_width = _widthLabel;
    }
    else
    {
	_width = _widthValue;
    }
}

void BubbleMenuRangeValueCompactGeometry::processEvent(InteractionEvent * event)
{
    if(event->asMouseEvent())
    {
        MouseInteractionEvent * mie = event->asMouseEvent();
        if(event->getInteraction() == BUTTON_DOWN
                || event->getInteraction() == BUTTON_DOUBLE_CLICK)
        {
            int x, y;

            x = mie->getX();
            y = mie->getY();

            _lastMouseX = x;
            _lastMouseY = y;
            return;
        }
        if(event->getInteraction() == BUTTON_DRAG
                || event->getInteraction() == BUTTON_UP)
        {
            int x, y;
            x = mie->getX();
            y = mie->getY();

            MenuRangeValueCompact * mrv = (MenuRangeValueCompact*)_item;

	    float min,max,current;

	    if(mrv->getIsLog())
	    {
		min = log(mrv->getMin()) / log(mrv->getLogBase());
		max = log(mrv->getMax()) / log(mrv->getLogBase());
		current = log(mrv->getValue()) / log(mrv->getLogBase());
	    }
	    else
	    {
		min = mrv->getMin();
		max = mrv->getMax();
		current = mrv->getValue();
	    }

            float pixelRange = 800;

            bool valueUpdated = false;
            if(x > _lastMouseX)
            {
                if(mrv->getValue() != mrv->getMax())
                {
                    float change = (x - _lastMouseX)
                            * (max - min) / pixelRange;
			    
		    float newValue;

		    if(mrv->getIsLog())
		    {
			newValue = pow(mrv->getLogBase(),current + change);
			newValue = std::max(newValue, mrv->getMin());
		    }
		    else
		    {
			newValue = std::max(mrv->getValue() + change,
                            mrv->getMin());
		    }
                    mrv->setValue(newValue);
                    valueUpdated = true;
                }
            }
            else if(x < _lastMouseX)
            {
                if(mrv->getValue() != mrv->getMin())
                {
                    float change = (x - _lastMouseX)
                            * (max - min) / pixelRange;
                    float newValue;
		    
		    if(mrv->getIsLog())
		    {
			newValue = pow(mrv->getLogBase(),current + change);
			newValue = std::min(newValue, mrv->getMax());
		    }
		    else
		    {
			newValue = std::min(mrv->getValue() + change,
                            mrv->getMax());
		    }
                    mrv->setValue(newValue);
                    valueUpdated = true;
                }
            }

            if(valueUpdated)
            {
                if(mrv->getCallback())
                {
                    mrv->getCallback()->menuCallback(_item);
                }
            }

            _lastMouseY = y;
            _lastMouseX = x;
            return;
        }
    }
    else if(event->asTrackedButtonEvent())
    {
        TrackedButtonInteractionEvent * tie = event->asTrackedButtonEvent();
        if(event->getInteraction() == BUTTON_DOWN
                || event->getInteraction() == BUTTON_DOUBLE_CLICK)
        {
            _point = tie->getTransform().getTrans();
            osg::Vec3 forward = osg::Vec3(0,1.0,0) * tie->getTransform();
            forward = forward - _point;
            _normal = forward ^ osg::Vec3(0,0,1.0);
            _normal.normalize();
            _lastDistance = 0.0;
            return;
        }
        if(event->getInteraction() == BUTTON_DRAG
                || event->getInteraction() == BUTTON_UP)
        {
            MenuRangeValueCompact * mrv = (MenuRangeValueCompact*)_item;

	    float min,max,current;

	    if(mrv->getIsLog())
	    {
		min = log(mrv->getMin()) / log(mrv->getLogBase());
		max = log(mrv->getMax()) / log(mrv->getLogBase());
		current = log(mrv->getValue()) / log(mrv->getLogBase());
	    }
	    else
	    {
		min = mrv->getMin();
		max = mrv->getMax();
		current = mrv->getValue();
	    }

            osg::Vec3 vec = tie->getTransform().getTrans();
            vec = vec - _point;
            float newDistance = vec * _normal;

            float range = 1000;

            bool valueUpdated = false;
            if(newDistance < _lastDistance)
            {
                if(mrv->getValue() != mrv->getMin())
                {
                    float change = (newDistance - _lastDistance)
                            * (max - min) / range;
                    float newValue;
		    
		    if(mrv->getIsLog())
		    {
			newValue = pow(mrv->getLogBase(),current + change);
			newValue = std::max(newValue, mrv->getMin());
		    }
		    else
		    {
			newValue = std::max(mrv->getValue() + change,
                            mrv->getMin());
		    }
                    mrv->setValue(newValue);
                    valueUpdated = true;
                }
            }
            else if(newDistance > _lastDistance)
            {
                if(mrv->getValue() != mrv->getMax())
                {
                    float change = (newDistance - _lastDistance)
                            * (max - min) / range;
                    float newValue;
		    
		    if(mrv->getIsLog())
		    {
			newValue = pow(mrv->getLogBase(),current + change);
			newValue = std::min(newValue, mrv->getMax());
		    }
		    else
		    {
			newValue = std::min(mrv->getValue() + change,
                            mrv->getMax());
		    }
                    mrv->setValue(newValue);
                    valueUpdated = true;
                }
            }

            if(valueUpdated)
            {
                if(mrv->getCallback())
                {
                    mrv->getCallback()->menuCallback(_item);
                }
            }

            _lastDistance = newDistance;

            return;
        }
    }
}

void BubbleMenuRangeValueCompactGeometry::showHoverText()
{
    if (_textGeode && _node)
    {
    _node->addChild(_textGeode);
    }
}

void BubbleMenuRangeValueCompactGeometry::hideHoverText()
{
    if (_textGeode && _node)
    {
    _node->removeChild(_textGeode);
    }
}
