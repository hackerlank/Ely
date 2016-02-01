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
 * \file /Ely/training/particles/SpriteParticleRendererExt.cpp
 *
 * \date 2016-01-31
 * \author consultit
 */

#include "Utilities/Tools.h"
#include "SpriteParticleRendererExt.h"

namespace ely
{

std::string SpriteParticleRendererExt::sourceTextureName = "",
		SpriteParticleRendererExt::sourceNodeName = "";
Filename SpriteParticleRendererExt::sourceFileName = "";

SpriteParticleRendererExt::SpriteParticleRendererExt()
{
}

SpriteParticleRendererExt::~SpriteParticleRendererExt()
{
}

const std::string& SpriteParticleRendererExt::getSourceTextureName()
{
	if (mSourceTextureName == "")
	{
		SpriteParticleRendererExt::sourceTextureName = DConfig::GetString(
				"particle-sprite-texture", "maps/lightbulb.rgb");
	}
	// Return instance copy of class variable
	return mSourceTextureName;
}

void SpriteParticleRendererExt::setSourceTextureName(const std::string& name)
{
	// Set instance copy of class variable
	mSourceTextureName = name;
}

bool SpriteParticleRendererExt::setTextureFromFile(const Filename& fileName)
{
	if (fileName == "")
	{
		fileName = getSourceTextureName();
	}

	Texture* t = TexturePool::load_texture(fileName);
	if (t != NULL)
	{
		set_texture(t, t->get_y_size());
		setSourceTextureName(fileName);
		return true;
	}

	PRINT_ERR_DEBUG("Couldn't find rendererSpriteTexture file: " << fileName);
	return false;
}

bool SpriteParticleRendererExt::addTextureFromFile(const Filename& fileName)
{
	if (get_num_anims() == 0)
	{
		return setTextureFromFile(fileName);
	}

	if (fileName == "")
	{
		fileName = getSourceTextureName();
	}
	Texture* t = TexturePool::load_texture(fileName);
	if (t != NULL)
	{
		add_texture(t, t->get_y_size());
		return true;
	}
	PRINT_ERR_DEBUG("Couldn't find rendererSpriteTexture file: " << fileName);
	return false;
}

const Filename& SpriteParticleRendererExt::getSourceFileName()
{
	if (mSourceFileName == "")
	{
		SpriteParticleRendererExt::sourceFileName = DConfig::GetString(
				"particle-sprite-model", "models/misc/smiley");
	}
	// Return instance copy of class variable
	return mSourceFileName;
}

void SpriteParticleRendererExt::setSourceFileName(const Filename& name)
{
	// Set instance copy of class variable
	mSourceFileName = name;
}

const std::string& SpriteParticleRendererExt::getSourceNodeName()
{
	if (mSourceNodeName == "")
	{
		SpriteParticleRendererExt::sourceNodeName = DConfig::GetString(
				"particle-sprite-node", "**/*");
	}
	// Return instance copy of class variable
	return mSourceNodeName;
}

void SpriteParticleRendererExt::setSourceNodeName(const std::string& name)
{
	// Set instance copy of class variable
	mSourceNodeName = name;
}

bool SpriteParticleRendererExt::setTextureFromNode(const std::string& modelName,
		const std::string& nodeName, bool sizeFromTexels)
{
	if (modelName == "")
	{
		modelName = getSourceFileName();
		if (nodeName == "")
		{
			nodeName = getSourceNodeName();
		}
	}
	///TODO
    // Load model and get texture
	PT(PandaNode) node = Loader().load_sync(modelName, LoaderOptions());
	NodePath("models");



    m = loader.loadModel(modelName)
    if (m == None):
        print "SpriteParticleRendererExt: Couldn't find model: %s!" % modelName
        return False

    np = m.find(nodeName)
    if np.isEmpty():
        print "SpriteParticleRendererExt: Couldn't find node: %s!" % nodeName
        m.removeNode()
        return False

    self.setFromNode(np, modelName, nodeName, sizeFromTexels)
    self.setSourceFileName(modelName)
    self.setSourceNodeName(nodeName)
    m.removeNode()
    return true;
}

bool SpriteParticleRendererExt::addTextureFromNode(const std::string& modelName,
		const std::string& nodeName, bool sizeFromTexels)
{
}

} /* namespace ely */
