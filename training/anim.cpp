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
 * \file /Ely/training/anim.cpp
 *
 * \date 30/giu/2012 (09:45:19)
 * \author marco
 */

#ifndef AUTO_BIND_H
#define AUTO_BIND_H

#include "pandabase.h"

#include "animControl.h"
#include "animControlCollection.h"

class PandaNode;

BEGIN_PUBLISH
////////////////////////////////////////////////////////////////////
//     Function: auto_bind
//  Description: Walks the scene graph or subgraph beginning at the
//               indicated node, and attempts to bind any AnimBundles
//               found to their matching PartBundles, when possible.
//
//               The list of all resulting AnimControls created is
//               filled into controls.
////////////////////////////////////////////////////////////////////
EXPCL_PANDA_CHAN void
auto_bind(PandaNode *root_node, AnimControlCollection &controls,
          int hierarchy_match_flags = 0);
END_PUBLISH

#endif

#include "animBundleNode.h"
#include "partBundleNode.h"
#include "config_chan.h"
#include "string_utils.h"
#include "partGroup.h"

typedef pset<AnimBundle *> AnimBundles;
typedef pmap<string, AnimBundles> Anims;

typedef pset<PartBundle *> PartBundles;
typedef pmap<string, PartBundles> Parts;


////////////////////////////////////////////////////////////////////
//     Function: bind_anims
//  Description: A support function for auto_bind(), below.  Given a
//               set of AnimBundles and a set of PartBundles that all
//               share the same name, perform whatever bindings make
//               sense.
////////////////////////////////////////////////////////////////////
static void
bind_anims(const PartBundles &parts, const AnimBundles &anims,
           AnimControlCollection &controls,
           int hierarchy_match_flags) {
  PartBundles::const_iterator pbi;

  for (pbi = parts.begin(); pbi != parts.end(); ++pbi) {
    PartBundle *part = (*pbi);
    AnimBundles::const_iterator abi;
    for (abi = anims.begin(); abi != anims.end(); ++abi) {
      AnimBundle *anim = (*abi);
      if (chan_cat.is_info()) {
        chan_cat.info()
          << "Attempting to bind " << *part << " to " << *anim << "\n";
      }

      PT(AnimControl) control =
        part->bind_anim(anim, hierarchy_match_flags);
      string name = (*abi)->get_name();
      if (name.empty()) {
        name = anim->get_name();
      }
      if (control != (AnimControl *)NULL) {
        if (controls.find_anim(name) != (AnimControl *)NULL) {
          // That name's already used; synthesize another one.
          int index = 0;
          string new_name;
          do {
            index++;
            new_name = name + '.' + format_string(index);
          } while (controls.find_anim(new_name) != (AnimControl *)NULL);
          name = new_name;
        }

        controls.store_anim(control, name);
      }

      if (chan_cat.is_info()) {
        if (control == (AnimControl *)NULL) {
          chan_cat.info()
            << "Bind failed.\n";
        } else {
          chan_cat.info()
            << "Bind succeeded, index "
            << control->get_channel_index() << "; accessible as "
            << name << "\n";
        }
      }
    }
  }
}

////////////////////////////////////////////////////////////////////
//     Function: r_find_bundles
//  Description: A support function for auto_bind(), below.  Walks
//               through the hierarchy and finds all of the
//               PartBundles and AnimBundles.
////////////////////////////////////////////////////////////////////
static void
r_find_bundles(PandaNode *node, Anims &anims, Parts &parts) {
  if (node->is_of_type(AnimBundleNode::get_class_type())) {
    AnimBundleNode *bn = DCAST(AnimBundleNode, node);
    AnimBundle *bundle = bn->get_bundle();
    anims[bundle->get_name()].insert(bundle);

  } else if (node->is_of_type(PartBundleNode::get_class_type())) {
    PartBundleNode *bn = DCAST(PartBundleNode, node);
    int num_bundles = bn->get_num_bundles();
    for (int i = 0; i < num_bundles; ++i) {
      PartBundle *bundle = bn->get_bundle(i);
      parts[bundle->get_name()].insert(bundle);
    }
  }

  PandaNode::Children cr = node->get_children();
  int num_children = cr.get_num_children();
  for (int i = 0; i < num_children; i++) {
    r_find_bundles(cr.get_child(i), anims, parts);
  }
}


////////////////////////////////////////////////////////////////////
//     Function: auto_bind
//  Description: Walks the scene graph or subgraph beginning at the
//               indicated node, and attempts to bind any AnimBundles
//               found to their matching PartBundles, when possible.
//
//               The list of all resulting AnimControls created is
//               filled into controls.
////////////////////////////////////////////////////////////////////
void
auto_bind(PandaNode *root_node, AnimControlCollection &controls,
          int hierarchy_match_flags) {
  // First, locate all the bundles in the subgraph.
  Anims anims;
  AnimBundles extra_anims;
  Parts parts;
  PartBundles extra_parts;
  r_find_bundles(root_node, anims, parts);

  if (chan_cat.is_debug()) {
    int anim_count = 0;
    Anims::const_iterator ai;
    for (ai = anims.begin(); ai != anims.end(); ++ai) {
      anim_count += (int)(*ai).second.size();
    }
    chan_cat.debug()
      << "Found " << anim_count << " anims:\n";
    for (ai = anims.begin(); ai != anims.end(); ++ai) {
      chan_cat.debug(false)
        << " " << (*ai).first;
      if ((*ai).second.size() != 1) {
        chan_cat.debug(false)
          << "*" << ((*ai).second.size());
      }
    }
    chan_cat.debug(false)
      << "\n";

    int part_count = 0;
    Parts::const_iterator pi;
    for (pi = parts.begin(); pi != parts.end(); ++pi) {
      part_count += (int)(*pi).second.size();
    }
    chan_cat.debug()
      << "Found " << part_count << " parts:\n";
    for (pi = parts.begin(); pi != parts.end(); ++pi) {
      chan_cat.debug(false)
        << " " << (*pi).first;
      if ((*pi).second.size() != 1) {
        chan_cat.debug(false)
          << "*" << ((*pi).second.size());
      }
    }
    chan_cat.debug(false)
      << "\n";
  }

  // Now, match up the bundles by name.

  Anims::const_iterator ai = anims.begin();
  Parts::const_iterator pi = parts.begin();

  while (ai != anims.end() && pi != parts.end()) {
    if ((*ai).first < (*pi).first) {
      // Here's an anim with no matching parts.
      if (hierarchy_match_flags & PartGroup::HMF_ok_wrong_root_name) {
        AnimBundles::const_iterator abi;
        for (abi = (*ai).second.begin(); abi != (*ai).second.end(); ++abi) {
          extra_anims.insert(*abi);
        }
      }
      ++ai;

    } else if ((*pi).first < (*ai).first) {
      // And here's a part with no matching anims.
      if (hierarchy_match_flags & PartGroup::HMF_ok_wrong_root_name) {
        PartBundles::const_iterator pbi;
        for (pbi = (*pi).second.begin(); pbi != (*pi).second.end(); ++pbi) {
          extra_parts.insert(*pbi);
        }
      }
      ++pi;

    } else {
      // But here we have (at least one) match!
      bind_anims((*pi).second, (*ai).second, controls,
                   hierarchy_match_flags);
      ++pi;

      // We don't increment the anim counter yet.  That way, the same
      // anim may bind to multiple parts, if they all share the same
      // name.
    }
  }

  if (hierarchy_match_flags & PartGroup::HMF_ok_wrong_root_name) {
    // Continue searching through the remaining anims and parts.

    while (ai != anims.end()) {
      // Here's an anim with no matching parts.
      if (hierarchy_match_flags & PartGroup::HMF_ok_wrong_root_name) {
        AnimBundles::const_iterator abi;
        for (abi = (*ai).second.begin(); abi != (*ai).second.end(); ++abi) {
          extra_anims.insert(*abi);
        }
      }
      ++ai;
    }

    while (pi != parts.end()) {
      // And here's a part with no matching anims.
      if (hierarchy_match_flags & PartGroup::HMF_ok_wrong_root_name) {
        PartBundles::const_iterator pbi;
        for (pbi = (*pi).second.begin(); pbi != (*pi).second.end(); ++pbi) {
          extra_parts.insert(*pbi);
        }
      }
      ++pi;
    }

    bind_anims(extra_parts, extra_anims, controls,
               hierarchy_match_flags);
  }
}

#include <iostream>
#include <string>
#include <vector>
#include <load_prc_file.h>
#include <auto_bind.h>
#include <partBundleHandle.h>
#include <character.h>
#include <animControlCollection.h>
#include <pandaFramework.h>
#include "Utilities/Tools.h"

//Bind the Model and the Animation
// don't use PT or CPT with AnimControlCollection
AnimControlCollection anim_collection;
AsyncTask::DoneStatus check_playing(GenericAsyncTask* task, void* data);

int main(int argc, char **argv)
{
	///setup
	// Load your configuration
	load_prc_file("config.prc");
	PandaFramework panda = PandaFramework();
	panda.open_framework(argc, argv);
	panda.set_window_title("animation training");
	WindowFramework* window = panda.open_window();
	if (window != (WindowFramework *) NULL)
	{
		std::cout << "Opened the window successfully!\n";
		// common setup
		window->enable_keyboard(); // Enable keyboard detection
		window->setup_trackball(); // Enable default camera movement
	}
	//Load the Actor Model
	NodePath Actor = window->load_model(window->get_render(),
			"bvw-f2004--airbladepilot/pilot-model");
	SMARTPTR(Character) character =
			DCAST(Character, Actor.find("**/+Character").node());
	SMARTPTR(PartBundle) pbundle = character->get_bundle(0);
	//Load Animations
	std::vector<std::string> animations;
	animations.push_back(std::string("pilot-chargeshoot"));
//	animations.push_back(std::string("pilot-crash"));
//	animations.push_back(std::string("pilot-chargewindup"));
	animations.push_back(std::string("pilot-discloop"));
//	animations.push_back(std::string("pilot-discwinddown"));
//	animations.push_back(std::string("pilot-discwindup"));
//	animations.push_back(std::string("pilot-firewinddown"));
//	animations.push_back(std::string("pilot-firewindup"));
//	animations.push_back(std::string("pilot-idle"));
//	animations.push_back(std::string("pilot-newdeath"));
//	animations.push_back(std::string("pilot-newidle"));
	/*
	 animations.push_back(std::string("pilot-pain"));
	 animations.push_back(std::string("pilot-fire"));
	 animations.push_back(std::string("pilot-charge"));
	 animations.push_back(std::string("pilot-chargeloop"));
	 */
	for (unsigned int i = 0; i < animations.size(); ++i)
	{
		window->load_model(Actor, "bvw-f2004--airbladepilot/" + animations[i]);
	}
	auto_bind(Actor.node(), anim_collection);
	pbundle->set_anim_blend_flag(true);
	pbundle->set_control_effect(anim_collection.get_anim(0), 0.5);
	pbundle->set_control_effect(anim_collection.get_anim(1), 0.5);
//	anim_collection.get_anim(0)->loop(true);
//	anim_collection.get_anim(1)->loop(true);
	int actualAnim = 0;
	//switch among animations
	AsyncTask* task = new GenericAsyncTask("check playing", &check_playing,
			reinterpret_cast<void*>(&actualAnim));
	task->set_delay(3);
	panda.get_task_mgr().add(task);
	// the name of an animation is preceded in the .egg file with <Bundle>:
	// loop a specific animation
//	anim_collection.loop(anim_collection.get_anim_name(1), true);
	// loop all animations
//	anim_collection.loop_all(true);
	// play an animation once:
//	anim_collection.play("panda_soft");
	// pose
//	anim_collection.pose("panda_soft", 5);

	//attach to scene
	Actor.reparent_to(window->get_render());
	Actor.set_pos(0.0, 100.0, -30.0);

	// Do the main loop
	panda.main_loop();
	// close the framework
	panda.close_framework();
	return 0;
}

AsyncTask::DoneStatus check_playing(GenericAsyncTask* task, void* data)
{
	//Control the Animations
	double time = ClockObject::get_global_clock()->get_real_time();
	int *actualAnim = reinterpret_cast<int*>(data);
	int num = *actualAnim % 3;
	if (num == 0)
	{
		std::cout << time << " - Blending" << std::endl;
		if (not anim_collection.get_anim(0)->is_playing())
		{
			anim_collection.get_anim(0)->play();
		}
		if (not anim_collection.get_anim(1)->is_playing())
		{
			anim_collection.get_anim(1)->play();
		}
	}
	else if (num == 1)
	{
		std::cout << time << " - Playing: " << anim_collection.get_anim_name(0)
				<< std::endl;
		if (not anim_collection.get_anim(0)->is_playing())
		{
			anim_collection.get_anim(0)->play();
		}
		if (anim_collection.get_anim(1)->is_playing())
		{
			anim_collection.get_anim(1)->stop();
		}
	}
	else
	{
		std::cout << time << " - Playing: " << anim_collection.get_anim_name(1)
				<< std::endl;
		anim_collection.get_anim(1)->play();
		if (anim_collection.get_anim(0)->is_playing())
		{
			anim_collection.get_anim(0)->stop();
		}
		if (not anim_collection.get_anim(1)->is_playing())
		{
			anim_collection.get_anim(1)->play();
		}
	}
	*actualAnim += 1;
//	task->set_delay(3);
	return AsyncTask::DS_again;
	//1
//	if (*actualAnim >= anim_collection.get_num_anims())
//	{
//		return AsyncTask::DS_done;
//	}
	//2
//	if (not anim_collection.is_playing())
//	{
//		if (*actualAnim >= anim_collection.get_num_anims())
//		{
//			*actualAnim = 0;
//		}
//		std::cout << anim_collection.get_anim_name(*actualAnim) << std::endl;
//		anim_collection.play(anim_collection.get_anim_name(*actualAnim));
//		*actualAnim += 1;
//	}
//	return AsyncTask::DS_cont;
}
