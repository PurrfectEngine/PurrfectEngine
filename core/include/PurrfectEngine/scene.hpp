#ifndef   PURRENGINE_SCENE_HPP_
#define   PURRENGINE_SCENE_HPP_

#include <vector>

#include "PurrfectEngine/object.hpp"

namespace PurrfectEngine {

  class purrScene {
  public:
    purrScene();
    purrScene(PUID uuid, std::vector<purrObject*> objects, purrObject *cameraObject);
    ~purrScene();

    bool addObject(purrObject *obj);
    bool addObjects(std::vector<purrObject*> objects);
    purrObject *getObject(PUID uuid);
    bool removeObject(PUID uuid);

    void setCamera(purrObject *object) { mCameraObject = object; }
    purrObject *getCamera() const { return mCameraObject; }

    purrObject *newObject();
  public:
    std::vector<purrObject*> getObjects() const { return mObjects; }
  private:
    PUID mUuid{};
    std::vector<PUID> mUuids{};
    std::vector<purrObject*> mObjects{};
    purrObject *mCameraObject = nullptr;
  };

}

#endif // PURRENGINE_SCENE_HPP_