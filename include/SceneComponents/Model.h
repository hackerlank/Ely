/*
 *   This file is part of Ely.
 *
 *   Ely is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   Ely is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with Ely.  If not, see <http://www.gnu.org/licenses/>.
 */
/**
 * \file /Ely/include/SceneComponents/Model.h
 *
 * \date 15/mag/2012 (15:32:23)
 * \author consultit
 */

#ifndef MODEL_H_
#define MODEL_H_

#include <list>
#include <nodePath.h>
#include <partBundle.h>
#include <animControlCollection.h>
#include "ObjectModel/Component.h"

namespace ely
{
class ModelTemplate;

/**
 * \brief Component representing the model and animations of an object.
 *
 * In relation to animations', this component accepts one model file, and
 * a number of animation files; it takes into account only the first
 * joint's hierarchy (PartBundle) found the model file, and the animations
 * (AnimBundle) found into the animation files (or model file) are bound to
 * this only joint's hierarchy.\n
 * Each animation can be referred by its associated file name. If more
 * animations exist in any file, the next ones are suffixed  with ".N"
 * where N=1,2,... .
 *
 * XML Param(s):
 * - "from_file"  			|single|"true"
 * - "scale"  				|single|"1.0,1.0,1.0"
 * - "model_file"  			|single|no default (can have this form: [anim_name1@
 * anim_name2@...@anim_nameN@]model_filename)
 * - "anim_files"  			|multiple|no default (each one specified as
 * "anim_name1@anim_file1[:anim_name2@anim_file2:...:anim_nameN@anim_fileN]"])
 * - "model_type"  			|single|no default
 * - "model_card_left"  	|single|no default
 * - "model_card_right"  	|single|no default
 * - "model_card_bottom"  	|single|no default
 * - "model_card_top"  		|single|no default
 *
 * \note parts inside [] are optional.\n
 */
class Model: public Component
{
protected:
	friend class ModelTemplate;

	virtual void reset();
	virtual bool initialize();
	virtual void onAddToObjectSetup();
	virtual void onRemoveFromObjectCleanup();
	virtual void onAddToSceneSetup();
	virtual void onRemoveFromSceneCleanup();

public:
	Model();
	Model(SMARTPTR(ModelTemplate)tmpl);
	virtual ~Model();

	virtual ComponentFamilyType familyType() const;
	virtual ComponentType componentType() const;

	/**
	 * \brief Gets/sets the node path associated to this model.
	 * @return The node path associated to this model.
	 */
	///@{
	NodePath getNodePath() const;
	void setNodePath(const NodePath& nodePath);
	///@}

	/**
	 * \brief Gets a reference to AnimControlCollection of this model.
	 *
	 * Thread safe.
	 * @return A reference to AnimControlCollection of this model.
	 */
	AnimControlCollection animations() const;

	/**
	 * \brief Returns a reference to the PartBundle animations are bound to.
	 *
	 * @return A reference to the PartBundle animations are bound to.
	 */
	SMARTPTR(PartBundle) getPartBundle() const;

private:
	///The NodePath associated to this model.
	NodePath mNodePath;

	///Flag indicating if component is set up from a file or procedurally.
	bool mFromFile;
	/**
	 * \name Main parameters.
	 */
	///@{
	///Model.
	std::string mModelNameParam;
	///Old owner object node path.
	NodePath mOldObjectNodePath;
	///Animations.
	std::list<std::string> mAnimFileListParam;
	///Scaling  (default: (1.0,1.0,1.0)).
	float mScale[3];
	///Type of model procedurally generated.
	std::string mModelTypeParam;
	///Card parameters.
	float mCardLeft, mCardRight, mCardBottom, mCardTop;
	///@}

	/**
	 * \brief Animations' related data/functions.
	 */
	///@{
	///The list of animations associated with this model.
	AnimControlCollection mAnimations;
	SMARTPTR(PartBundle)mFirstPartBundle;
	typedef std::set<SMARTPTR(AnimBundle)> AnimBundles;
	typedef std::map<string, AnimBundles> Anims;
	typedef std::set<SMARTPTR(PartBundle)> PartBundles;
	typedef std::map<string, PartBundles> Parts;
	/**
	 * \brief A support function for auto_bind().
	 *
	 * Walks through the hierarchy and finds all of the PartBundles
	 * and AnimBundles.
	 * @param node The panda node.
	 * @param anims Out parameter: the table of AnimBundle sets indexed
	 * by their names.
	 * @param parts Out parameter: the table of AnimBundle sets indexed
	 * by their names.
	 */
	void do_r_find_bundles(SMARTPTR(PandaNode) node, Anims &anims, Parts &parts);
	///@}

	///TypedObject semantics: hardcoded
public:
	static TypeHandle get_class_type()
	{
		return _type_handle;
	}
	static void init_type()
	{
		Component::init_type();
		register_type(_type_handle, "Model", Component::get_class_type());
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

///inline definitions

inline void Model::reset()
{
	//
	mNodePath = NodePath();
	mFromFile = false;
	mModelNameParam.clear();
	mOldObjectNodePath = NodePath();
	mAnimFileListParam.clear();
	mScale[0] = mScale[1] = mScale[2] = 1.0;
	mModelTypeParam.clear();
	mCardLeft = mCardRight = mCardBottom = mCardTop = 0.0;
	mAnimations.clear_anims();
	mFirstPartBundle.clear();
}

inline AnimControlCollection Model::animations() const
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	return mAnimations;
}

inline void Model::onAddToSceneSetup()
{
}

inline void Model::onRemoveFromSceneCleanup()
{
}

inline NodePath Model::getNodePath() const
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	return mNodePath;
}

inline void Model::setNodePath(const NodePath& nodePath)
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	mNodePath = nodePath;
}

}  // namespace ely

#endif /* MODEL_H_ */
