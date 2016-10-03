/**
 * \file p3Sound3d.h
 *
 * \date 2016-09-30
 * \author consultit
 */

#ifndef P3SOUND3D_H_
#define P3SOUND3D_H_

#include "audio_includes.h"
#include "gameAudioManager.h"

#ifndef CPPPARSER
#include "support/common.h"
#endif //CPPPARSER

/**
 * P3Sound3d is a PandaNode class designed for manipulating 3d sounds attached
 * to an object.\n
 *
 * P3Sound3d both maintains the set of sounds attached to an object and manages
 * their automatic position/velocity update.\n
 * Sounds can be dynamically loaded/unloaded.\n
 * Only playing sounds will have their position/velocity updated.\n
 * For "static" objects sound position/velocity can be set directly and is not
 * automatically updated.\n
 * All updates is done with respect to reference node.
 *
 * > **P3Sound3d text parameters**:
 * param | type | default | note
 * ------|------|---------|-----
 * | *static*  				|single| *false* | -
 * | *min_distance* 		|single| *3.28* | -
 * | *max_distance* 		|single| *32.8* | -
 * | *sound_files* 			|multiple| - | each one specified as "sound_name1@sound_file1[:sound_name2@sound_file2:...:sound_nameN@sound_fileN]"
 *
 * \note parts inside [] are optional.\n
 */
class EXPORT_CLASS P3Sound3d: public PandaNode
{
public:
	///sound, sound file table
	typedef pmap<string, pair<PT(AudioSound), string> > SoundTable;

PUBLISHED:

	// To avoid interrogatedb warning.
#ifdef CPPPARSER
	virtual ~P3Sound3d();
#endif //CPPPARSER

	/**
	 * \name REFERENCE NODE
	 */
	///@{
	INLINE void set_reference_node_path(const NodePath& reference);
	///@}

	/**
	 * \name SOUND3D
	 */
	///@{
	void update(float dt);
	///@}

	/**
	 * \name AUDIO SOUNDS
	 */
	///@{
	int add_sound(const string& soundName, const string& fileName);
	int remove_sound(const string& soundName);
	PT(AudioSound) get_sound_by_name(const string& soundName) const;
	PT(AudioSound) get_sound(int index) const;
	INLINE int get_num_sounds() const;
	MAKE_SEQ(get_sounds, get_num_sounds, get_sound);
	INLINE PT(AudioSound) operator [](int index) const;
	INLINE int size() const;
	///@}

	/**
	 * \name PARAMETERS' GETTERS/SETTERS
	 */
	///@{
	void set_static(bool enable);
	INLINE bool get_static() const;
	void set_min_distance(float dist);
	INLINE float get_min_distance() const;
	void set_max_distance(float dist);
	INLINE float get_max_distance() const;
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
	typedef void (*UPDATECALLBACKFUNC)(PT(P3Sound3d));
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
	friend void unref_delete<P3Sound3d>(P3Sound3d*);
	friend class GameAudioManager;

	P3Sound3d(const string& name);
	virtual ~P3Sound3d();

private:
	///This NodePath.
	NodePath mThisNP;
	///The reference node path.
	NodePath mReferenceNP;
	///Static flag.
	bool mStatic;
	///The set of sounds attached to this component.
	SoundTable mSounds;
	/// Sounds' characteristics.
	///@{
	float mMinDist, mMaxDist;
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
	P3Sound3d(const P3Sound3d&);
	P3Sound3d& operator=(const P3Sound3d&);

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
		register_type(_type_handle, "P3Sound3d", PandaNode::get_class_type());
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

INLINE ostream &operator << (ostream &out, const P3Sound3d & sound3d);

///inline
#include "p3Sound3d.I"

#endif /* P3SOUND3D_H_ */
