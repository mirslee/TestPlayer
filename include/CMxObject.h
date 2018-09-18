#ifndef CMXOBJECT_H
#define CMXOBJECT_H

/* Object flags */
#define OBJECT_FLAGS_QUIET       0x0002
#define OBJECT_FLAGS_NOINTERACT  0x0004
#define OBJECT_FLAGS_INSECURE    0x1000

class CMxLib;
class CMxObject;

struct MxCommonMembers
{
    /** Object type name
     *
     * A constant string identifying the type of the object (for logging)
     */
    const char *object_type;
    
    /** Log messages header
     *
     * Human-readable header for log messages. This is not thread-safe and
     * only used by VLM and Lua interfaces.
     */
    char *header;
    
    int  flags;
    
    /** Module probe flag
     *
     * A boolean during module probing when the probe is "forced".
     * See \ref module_need().
     */
    bool force;
    
    /** LibVLC instance
     *
     * Root VLC object of the objects tree that this object belongs in.
     */
    CMxLib *libvlc;
    
    /** Parent object
     *
     * The parent object in the objects tree. For the root object, this is NULL.
     */
    CMxObject *parent;
};

/**
 * Backward compatibility macro
 */
#define MX_COMMON_MEMBERS struct MxCommonMembers obj;

class CMxLib
{
    MX_COMMON_MEMBERS
};

class CMxObject {
    MX_COMMON_MEMBERS
};


#endif
