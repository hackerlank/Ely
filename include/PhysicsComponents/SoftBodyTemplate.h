/*
 * SoftBodyTemplate.h
 *
 *  Created on: 08/ott/2013
 *      Author: marco
 */

#ifndef SOFTBODYTEMPLATE_H_
#define SOFTBODYTEMPLATE_H_

#include "ObjectModel/ComponentTemplate.h"

namespace ely
{
class SoftBodyTemplate: public ComponentTemplate
{
protected:

	virtual SMARTPTR(Component)makeComponent(const ComponentId& compId);

public:
	SoftBodyTemplate(PandaFramework* pandaFramework,
			WindowFramework* windowFramework);
	virtual ~SoftBodyTemplate();

	virtual ComponentType componentType() const;
	virtual ComponentFamilyType familyType() const;

	virtual void setParametersDefaults();

private:

	///TypedObject semantics: hardcoded
public:
	static TypeHandle get_class_type()
	{
		return _type_handle;
	}

	static void init_type()
	{
		ComponentTemplate::init_type();
		register_type(_type_handle, "SoftBodyTemplate",
				ComponentTemplate::get_class_type());
	}

	virtual TypeHandle get_type() const
	{
		return get_class_type();
	}

	virtual TypeHandle force_init_type()
	{
		init_type();
		return get_class_type();
	}

private:
	static TypeHandle _type_handle;
};
} /* namespace ely */
#endif /* SOFTBODYTEMPLATE_H_ */
