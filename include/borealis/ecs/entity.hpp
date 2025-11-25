#if !defined(ENTITY_HPP)
#define ENTITY_HPP

#include <borealis/util/util.h>

namespace brl
{
    struct EcsEntity {
        std::string name;

        glm::vec3 localPosition = glm::vec3(0);
        glm::quat localRotation = glm::identity<glm::quat>();
        glm::vec3 localScale = glm::vec3(1.0);

        glm::vec3 position();
        glm::quat rotation();
        glm::vec3 scale();
        void lookAt(glm::vec3 point, glm::vec3 up = glm::vec3{0,1,0});
        void setEulerAngles(glm::vec3 euler);

        void destroy();
        void setActive(bool active);

        bool isSelfActive();
        bool isGlobalActive();

        void setParent(EcsEntity* e);

        template <typename T>
        T* getEntityInParent();

        template <typename T>
        T* getEntityInChildren();

        EcsEntity();

    protected:
        friend struct EcsEngine;
        virtual void awake();
        virtual void start();
        virtual void earlyUpdate();
        virtual void update();
        virtual void lateUpdate();
        virtual void fixedUpdate();

        virtual void onEnable();
        virtual void onDisable();
        virtual void onDestroy();

        void internalDestroy();

        glm::mat4 calculateTransform();

        std::vector<EcsEntity*> children;
        EcsEntity* parent = nullptr;
        bool active = true;

        bool started = false;
    };

    template <typename T>
    T* EcsEntity::getEntityInParent()
    {
        if (parent)
        {
            if (typeid(*parent) == typeid(T))
            {
                return dynamic_cast<T*>(parent);
            }

            auto p = parent->getEntityInParent<T>();
            if (p)
                return p;
        }

        return nullptr;
    }

    template <typename T>
    T* EcsEntity::getEntityInChildren()
    {
        return nullptr;
    }

    struct EcsEngine {

        EcsEngine();

        void update();
        void shutdown();

    private:
        friend EcsEntity;

        static EcsEngine* instance;

        void destroyEntity(EcsEntity* e);


        std::vector<EcsEntity*> entities;
    };
} // namespace brl


#endif // ENTITY_HPP
