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
#include <texturePool.h>
#include <loader.h>

namespace ely
{

std::string SpriteParticleRendererExt::sourceTextureName = "",
		SpriteParticleRendererExt::sourceFileName = "",
		SpriteParticleRendererExt::sourceNodeName = "";

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

bool SpriteParticleRendererExt::setTextureFromFile(const std::string& _fileName)
{
	std::string fileName;
	if (_fileName == "")
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

bool SpriteParticleRendererExt::addTextureFromFile(const std::string& _fileName)
{
	if (get_num_anims() == 0)
	{
		return setTextureFromFile(_fileName);
	}
	std::string fileName;
	if (_fileName == "")
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

const std::string& SpriteParticleRendererExt::getSourceFileName()
{
	if (mSourceFileName == "")
	{
		SpriteParticleRendererExt::sourceFileName = DConfig::GetString(
				"particle-sprite-model", "models/misc/smiley");
	}
	// Return instance copy of class variable
	return mSourceFileName;
}

void SpriteParticleRendererExt::setSourceFileName(const std::string& name)
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
} /*namespace ely*/

namespace
{
inline NodePath loadModel(const std::string& modelPath)
{
	NodePath nodePath = NodePath();
	PT(PandaNode)node = Loader("loader").load_sync(Filename(modelPath));
	if (node)
	{
		NodePath nodePath(node);
	}
	return nodePath;
}
}  // namespace

namespace ely
{
bool SpriteParticleRendererExt::setTextureFromNode(
		const std::string& _modelName, const std::string& _nodeName,
		bool sizeFromTexels)
{
	std::string modelName, nodeName;
	if (_modelName == "")
	{
		modelName = getSourceFileName();
		if (_nodeName == "")
		{
			nodeName = getSourceNodeName();
		}
	}

	// Load model and get texture
	NodePath m = loadModel(modelName);
	if (m == NodePath())
	{
		PRINT_ERR_DEBUG(
				"SpriteParticleRendererExt: Couldn't find model: " << modelName);
		return false;
	}

	NodePath np = m.find(nodeName);
	if (np.is_empty())
	{
		PRINT_ERR_DEBUG(
				"SpriteParticleRendererExt: Couldn't find node: " << nodeName);
		m.remove_node();
		return false;
	}
	set_from_node(np, modelName, nodeName, sizeFromTexels);
	setSourceFileName(modelName);
	setSourceNodeName(nodeName);
	m.remove_node();
	return true;
}

bool SpriteParticleRendererExt::addTextureFromNode(const std::string& _modelName,
		const std::string& _nodeName, bool sizeFromTexels)
{
	if (get_num_anims() == 0)
	{
		return setTextureFromNode(_modelName, _nodeName, sizeFromTexels);
	}
	std::string modelName, nodeName;
	if (_modelName == "")
	{
		modelName = getSourceFileName();
		if (_nodeName == "")
		{
			nodeName = getSourceNodeName();
		}
	}

	// Load model and get texture
	NodePath m = loadModel(modelName);
	if (m == NodePath())
	{
		PRINT_ERR_DEBUG(
				"SpriteParticleRendererExt: Couldn't find model: " << modelName);
		return false;
	}
	NodePath np = m.find(nodeName);
	if (np.is_empty())
	{
		PRINT_ERR_DEBUG(
				"SpriteParticleRendererExt: Couldn't find node: " << nodeName);
		m.remove_node();
		return false;
	}

	add_from_node(np, modelName, nodeName, sizeFromTexels);
	m.remove_node();

	return true;
}

} /* namespace ely */
