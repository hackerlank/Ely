/**
 * \file p3Listener.h
 *
 * \date 2016-09-30
 * \author consultit
 */

#ifndef P3LISTENER_H_
#define P3LISTENER_H_

#include "audio_includes.h"
#include "gameAudioManager.h"

#ifndef CPPPARSER
#include "support/common.h"
#endif //CPPPARSER

/**
 * \brief Component manipulating the listener for 3d sounds.
 *
 * > **XML Param(s)**:
 * param | type | default | note
 * ------|------|---------|-----
 * | *static*  				|single| *false* | -
 *
 * \note parts inside [] are optional.\n
 */
/**
 * P3Listener is a PandaNode class designed to make an object a listener of another
 * object.
 *
 * P3Listener can be enabled/disabled as a whole (enabled by default).\n
 * P3Listener can handle basic rotations movement (head_left, head_right,
 * pitch_left, pitch_right) only if it is enabled to do so by calling the
 * corresponding "enabler". In turn, an enabled basic rotation movement can be
 * activated/deactivated through the corresponding "activator".\n
 * Rotation is updated relative to:
 * - z local axis, ie head (yaw)
 * - x local axis, ie pitch
 * Rotation through y local axis (roll) is not considered.\n
 * Chasing can be fixed or dampened, from behind or from the front.\n
 * With dampened chasing various movement's parameters (like friction, min/max
 * distance, min/max height etc...) can be set.\n
 * A task could update the position/orientation of the attached PandaNode object
 * based on the current position/orientation of the chased object, and the
 * currently enabled basic rotation movements, by calling the "update()"
 * method.\n
 * Usually movements are activated/deactivated through callback associated to
 * events (keyboard, mouse etc...).\n
 * Since in Panda3d by default, "mouse-move" events are not defined, mouse
 * movements can be enabled/disabled as "implicit activators" of head/pitch
 * basic rotations by calling "enable_mouse_head()"/"enable_mouse_pitch()"
 * methods. In this way head/pitch basic rotations can be activated
 * independently through mouse movements and/or normal activator methods.\n
 * On the other hand, "mouse-move" event could be defined by using
 * \code
 * ButtonThrower::set_move_event()
 * \endcode
 * (\see: http://www.panda3d.org/forums/viewtopic.php?t=9326 and
 * \see: http://www.panda3d.org/forums/viewtopic.php?t=6049), and in this case
 * if an application wishes to handle directly these events, it has to disable
 * the previously described mouse movements handling by calling:
 * \code
 * enable_mouse_move(true)
 * \endcode
 * The up axis is the "z" axis.
 * Rotation movements can be inverted (default: not inverted).\n
 *
 * > **P3Listener text parameters**:
 * param | type | default | note
 * ------|------|---------|-----
 * | *enabled*  				|single| *true* | -
 * | *backward*					|single| *true* | -
 * | *fixed_relative_position*	|single| *true* | -
 * | *max_distance*				|single| - | -
 * | *min_distance*				|single| - | -
 * | *max_height*				|single| - | -
 * | *min_height*				|single| - | -
 * | *friction*					|single| 1.0 | -
 * | *fixed_look_at*			|single| *true* | -
 * | *look_at_distance*			|single| - | -
 * | *look_at_height*			|single| - | -
 * | *mouse_move*  				|single| *disabled* | -
 * | *mouse_head*  				|single| *disabled* | -
 * | *mouse_pitch*  			|single| *disabled* | -
 * | *head_left*  				|single| *enabled* | -
 * | *head_right*  				|single| *enabled* | -
 * | *pitch_up*  				|single| *enabled* | -
 * | *pitch_down*  				|single| *enabled* | -
 * | *sens_x*  					|single| 0.2 | -
 * | *sens_y*  					|single| 0.2 | -
 * | *inverted_rotation*		|single| *false* | -
 *
 * \note parts inside [] are optional.\n
 */
class EXPORT_CLASS P3Listener: public PandaNode
{
PUBLISHED:

	// To avoid interrogatedb warning.
#ifdef CPPPARSER
	virtual ~P3Listener();
#endif //CPPPARSER

	/**
	 * \name REFERENCE NODE
	 */
	///@{
	INLINE void set_reference_node_path(const NodePath& reference);
	///@}

	/**
	 * \name LISTENER
	 */
	///@{
	void update(float dt);
	///@}

	/**
	 * \name PARAMETERS' GETTERS/SETTERS
	 */
	///@{
	void set_static(bool enable);
	INLINE bool get_static() const;
	///@}

	/**
	 * \name OUTPUT
	 */
	///@{
	void output(ostream &out) const;
	///@}

#if defined(PYTHON_BUILD) || defined(CPPPARSER)
	/**
	 * \name PYTHON UPDATE CALLBACK
	 */
	///@{
	void set_update_callback(PyObject *value);
	///@}
#else
	/**
	 * \name C++ UPDATE CALLBACK
	 */
	///@{
	typedef void (*UPDATECALLBACKFUNC)(PT(P3Listener));
	void set_update_callback(UPDATECALLBACKFUNC value);
	///@}
#endif //PYTHON_BUILD

public:
	/**
	 * \name C++ ONLY
	 * Library & support low level related methods.
	 */
	///@{
	///@}

protected:
	friend void unref_delete<P3Listener>(P3Listener*);
	friend class GameAudioManager;

	P3Listener(const string& name);
	virtual ~P3Listener();

private:
	///The reference node path.
	NodePath mReferenceNP;
	///Static flag.
	bool mStatic;
	/// Sounds' characteristics.
	///@{
	LPoint3f mPosition;
	///@}

	inline void do_reset();
	void do_initialize();
	void do_finalize();


	/**
	 * \name Helpers variables/functions.
	 */
	///@{
	void do_set_3d_static_attributes();
	///@}

#if defined(PYTHON_BUILD) || defined(CPPPARSER)
	/**
	 * \name Python callback.
	 */
	///@{
	PyObject *mSelf;
	PyObject *mUpdateCallback;
	PyObject *mUpdateArgList;
	///@}
#else
	/**
	 * \name C++ callback.
	 */
	///@{
	UPDATECALLBACKFUNC mUpdateCallback;
	///@}
#endif //PYTHON_BUILD

	// Explicitly disabled copy constructor and copy assignment operator.
	P3Listener(const P3Listener&);
	P3Listener& operator=(const P3Listener&);

public:
	/**
	 * \name TypedWritable API
	 */
	///@{
	static void register_with_read_factory();
	virtual void write_datagram (BamWriter *manager, Datagram &dg) override;
	virtual int complete_pointers(TypedWritable **p_list, BamReader *manager) override;
	///@}

protected:
	static TypedWritable *make_from_bam(const FactoryParams &params);
	virtual void fillin(DatagramIterator &scan, BamReader *manager) override;

public:
	/**
	 * \name TypedObject API
	 */
	///@{
	static TypeHandle get_class_type()
	{
		return _type_handle;
	}
	static void init_type()
	{
		PandaNode::init_type();
		register_type(_type_handle, "P3Listener", PandaNode::get_class_type());
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
	///@}

private:
	static TypeHandle _type_handle;

};

INLINE ostream &operator << (ostream &out, const P3Listener & listener);

///inline
#include "p3Listener.I"

#endif /* P3LISTENER_H_ */
