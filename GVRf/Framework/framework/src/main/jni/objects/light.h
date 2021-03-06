/* Copyright 2015 Samsung Electronics Co., LTD
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/***************************************************************************
 * Class containing light source parameters.
 ***************************************************************************/

#ifndef LIGHT_H_
#define LIGHT_H_

#include <map>
#include <memory>
#include <string>

#include "glm/glm.hpp"

#include "objects/hybrid_object.h"
#include "../objects/scene_object.h"
#include "components/component.h"
#include "util/gvr_jni.h"
#include "objects/material.h"
#include "../engine/renderer/renderer.h"
#include "glm/gtc/matrix_inverse.hpp"
namespace gvr {
class Color;
class SceneObject;
class Scene;
class ShaderManager;

class Light: public Component {
public:
    static const int MAX_SHADOW_MAPS;
    static const int SHADOW_MAP_SIZE;

    explicit Light()
    :   Component(Light::getComponentType()),
        enabled_(true),
        parent_(nullptr),
        shadowMaterial_(nullptr),
 		fboId_(-1),
		shadowMapIndex_(-1) {
    }

    ~Light() {
    }

    static long long getComponentType() {
        return (long long) &getComponentType;
    }

    bool enabled() {
        return enabled_;
    }

    void enable() {
        enabled_ = true;
        setDirty();
    }

    void disable() {
        setDirty();
    }

    float getFloat(std::string key) {
        auto it = floats_.find(key);
        if (it != floats_.end()) {
            return it->second;
        } else {
            std::string error = "Light::getFloat() : " + key + " not found";
            throw error;
        }
    }

    void setFloat(std::string key, float value) {
        floats_[key] = value;
        if (enabled_) {
            setDirty();
        }
    }

    glm::vec3 getVec3(std::string key) {
        auto it = vec3s_.find(key);
        if (it != vec3s_.end()) {
            return it->second;
        } else {
            std::string error = "Light::getVec3() : " + key + " not found";
            throw error;
        }
    }

    void setVec3(std::string key, glm::vec3 vector) {
        vec3s_[key] = vector;
        if (enabled_) {
            setDirty();
        }
    }

    glm::vec4 getVec4(std::string key) {
        auto it = vec4s_.find(key);
        if (it != vec4s_.end()) {
            return it->second;
        } else {
            std::string error = "Light::getVec4() : " + key + " not found";
            throw error;
        }
    }

    void setVec4(std::string key, glm::vec4 vector) {
        vec4s_[key] = vector;
        if (enabled_) {
            setDirty();
        }
    }

    bool getMat4(std::string key, glm::mat4& matrix) {
        auto it = mat4s_.find(key);
        if (it != mat4s_.end()) {
            matrix = it->second;
            return true;
        }
        return false;
    }

    void setMat4(std::string key, glm::mat4 matrix) {
        mat4s_[key] = matrix;
        if (enabled_) {
            setDirty();
        }
    }
    Material* getShadowMaterial(){
    	return shadowMaterial_;
    }

    bool castShadow() {
         return shadowMaterial_ != NULL;
    }

    /**
     * Enables or disables shadow casting.
     *
     * If shadows are enabled, makeShadowMap will compute a shadow map
     * by rendering the scene from the viewpoint of the light and
     * bindShadowMaps will bind the resulting framebuffer as a
     * texture on the light.
     */
    void castShadow(Material* material) {
        shadowMaterial_ = material;
        setDirty();
    }


    /**
     * Internal function called at the start of each shader
     * to update the light uniforms (if necessary).
     * @param program   ID of GL shader program
     * @param texIndex  GL texture index for shadow map
     */
    void render(int program, int texIndex);

    /**
     * Internal function called at the start of each frame
     * to update the shadow map.
     */
    bool makeShadowMap(Scene* scene, ShaderManager* shader_manager, int texIndex, std::vector<SceneObject*>& scene_objects, int, int);

    /**
     * Internal function called during rendering to bind the shadow map
     * framebuffer to the texture for this light.
     */
    static void bindShadowMap(int program, int texIndex);

    /***
     * Creates the storage for shadow maps
     */
    void static createDepthTexture(int width, int height, int depth);

    std::string getLightID() {
        return lightID_;
    }

     /**
     * Set the light ID. This is a string that uniquely
     * identifies this light. This ID is generated by
     * GVRScene when the light is attached.
     * {@link GVRScene.addLight }
     */
    void setLightID(std::string lightid) {
        lightID_ = lightid;
    };

private:
    Light(const Light& light);
    Light(Light&& light);
    Light& operator=(const Light& light);
    Light& operator=(Light&& light);

    /*
     * Generate the framebuffer used for shadow map generation
     */
    void generateFBO();

    /*
     * Mark the light as needing update for all shaders using it
     */
    void setDirty() {
        for (auto it = dirty_.begin(); it != dirty_.end(); ++it) {
            it->second = true;
        }
    }

    /*
     * Get the GL uniform offset for a named uniform.
     */
    int getOffset(std::string key, int programId) {
        auto it = offsets_.find(key);
        if (it != offsets_.end()) {
            std::map<int, int> offsets = it->second;
            auto it2 = offsets.find(programId);
            if (it2 != offsets.end()) {
                return it2->second;
            }
        }
        return -1;
    }

private:
    bool enabled_;
    int size_;
    int shadowMapIndex_;
    GLuint fboId_;
    std::string lightID_;
    SceneObject* parent_;
    Material* shadowMaterial_;
    std::map<int, bool> dirty_;
    glm::mat4 shadow_matrix_;
    std::map<std::string, float> floats_;
    std::map<std::string, glm::vec3> vec3s_;
    std::map<std::string, glm::vec4> vec4s_;
    std::map<std::string, glm::mat4> mat4s_;
    std::map<std::string, std::map<int, int> > offsets_;
    std::map<std::string, Texture*> textures_;
    static GLTexture* depth_texture_;
    static GLTexture* color_texture_;
};
}
#endif
