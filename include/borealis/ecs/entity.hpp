#if !defined(ENTITY_HPP)
#define ENTITY_HPP

#include <borealis/util/util.h>

namespace brl
{
    struct EcsEntity {
        /**
         * @brief 
         * This parameter holds the name for the entity
         */
        std::string name;

        /**
         * @brief 
         * This parameter holds the positional value for the entity
         */
        glm::vec3 localPosition = glm::vec3(0);

        /**
         * @brief 
         * This parameter holds the rotational value for the entity
         */
        glm::quat localRotation = glm::identity<glm::quat>();

        /**
         * @brief 
         * This parameter holds the scaling value for the entity
         */
        glm::vec3 localScale = glm::vec3(1.0);

        /**
         * @brief 
         * This function returns the global positional value for the entity
         * @return glm::vec3 The value.
         */
        virtual glm::vec3 position();

        /**
         * @brief 
         * This function returns the global rotational value for the entity
         * @return glm::quat The returned value.
         */
        glm::quat rotation();

        /**
         * @brief 
         * This function returns the global scaling value for the entity.
         * @return glm::vec3 The returned value.
         */
        virtual glm::vec3 scale();

        /**
         * @brief 
         * This function forces the entity to look at a specific point.
         * @param point The point in space that the entity is forced to look at
         * @param up The Up Vector used in rotation calculations
         */
        void lookAt(glm::vec3 point, glm::vec3 up = glm::vec3{0,1,0});

        /**
         * @brief Set the Euler Angles object
         * This function converts the euler value to quaternions and sets the rotational value to the calculated quaternion
         * @param euler The euler value.
         */
        void setEulerAngles(glm::vec3 eulerAngles);

        /**
         * @brief 
         * This function removes the entity from the simulation world.
         */
        void destroy();

        /**
         * @brief Set the Active object
         * This function disables/enables the entity based on the active value.
         * @param active The value that determines whether the entity is active or not.
         */
        void setActive(bool activeValue);

        /**
         * @brief 
         * This function returns the activity value of the entity.
         * @return true The entity is active.
         * @return false The entity is inactive.
         */
        bool isSelfActive();

        /**
         * @brief 
         * This function returns the activity value of the entity, and all of it's ancestors
         * @return true The entity and all of it's ancestors are active.
         * @return false Either the entity, or one of it's ancestors are inactive.
         */
        bool isGlobalActive();

        /**
         * @brief Set the Parent object
         * This function sets the entity's parent object.
         * @param e The parent that the entity will have.
         */
        void setParent(EcsEntity* entity);

        /**
         * @brief Get the Entity In Parent object
         * This function returns a possibility of an entity type that is present in it's parents.
         * @tparam T The entity type to be searched for.
         * @return T* The returned ancestor of the clarified entity type.
         */
        template <typename T>
        T* getEntityInParent();

        /**
         * @brief Get the Entity In Children object
         * This function returns a possibility of an entity type that is present in it's children.
         * @tparam T The entity type to be searched for.
         * @return T* The returned descendant of the clarified entity type
         */
        template <typename T>
        T* getEntityInChildren();

        /**
         * @brief Get the Entity object
         * This function returns the entity object converted to a clarified entity type
         * @tparam T The entity type to be converted to.
         * @return T* The returned entity version of the clarified entity type
         */
        template <typename T>
        T* getEntity();

        /**
         * This function returns a child at the index provided
         * @param index The index of the child to be searched for
         * @return The child that is either found or not
         */
        EcsEntity* getChild(int index);

        /**
         * This function returns all of the children of this object
         * @return The children of the object
         */
        EcsEntity** getChildren(int& count);

        EcsEntity();

    protected:
        friend struct EcsEngine;

        /**
         * @brief 
         * This function is ran on the first frame the entity is initialized.
         */
        virtual void awake();

        /**
         * @brief 
         * This function is ran on the first update the entity is initialized.
         */
        virtual void start();

        /**
         * @brief 
         * This function is ran before the main update loop.
         */
        virtual void earlyUpdate();

        /**
         * @brief 
         * This function is ran in the update loop
         */
        virtual void update();

        /**
         * @brief 
         * This function is ran after the update loop. It is usually ran after physics calculations.
         */
        virtual void lateUpdate();

        /**
         * @brief 
         * This function is ran alongside physics calculations.
         */
        virtual void fixedUpdate();

        /**
         * @brief 
         * This function is ran when the entity object is enabled.
         */
        virtual void onEnable();

        /**
         * @brief 
         * This function is ran when the entity object is disabled.
         */
        virtual void onDisable();

        /**
         * @brief 
         * This function is ran when the entity is destroyed.
         */
        virtual void onDestroy();

        void internalDestroy();

        /**
         * @brief 
         * This function returns a composite matrix of all the transformations.
         * @return glm::mat4 The calculated matrix.
         */
        virtual glm::mat4 calculateTransform();

        /**
         * @brief 
         * This is a list of all the children of the entity.
         */
        std::vector<EcsEntity*> children;

        /**
         * @brief 
         * This is the parent object of the entity;
         */
        EcsEntity* parent = nullptr;

        /**
         * @brief 
         * This is the boolean that returns if the entity is active or not.
         */
        bool active = true;

        bool started = false;
    };

    template <typename T>
    T* EcsEntity::getEntityInParent()
    {
        if (parent)
        {
            if (typeid(*parent).hash_code() == typeid(T).hash_code())
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
        for (auto child : children)
        {
            if (typeid(*child).hash_code() == typeid(T).hash_code())
            {
                return dynamic_cast<T*>(child);
            }
            
            auto p = child->getEntityInChildren<T>();
            if (p)
                return p;


        }

        return nullptr;
    }

    template <typename T>
    T* EcsEntity::getEntity()
    {
        return dynamic_cast<T*>(this);
    }

    struct EcsEngine {

        EcsEngine();

        /**
         * @brief 
         * This function runs the full entity-based update sequence.
         */
        void update();

        /**
         * @brief 
         * This function destroys all of the entities and shutdowns the engine.
         */
        void shutdown();

    private:
        friend EcsEntity;

        static EcsEngine* instance;

        void destroyEntity(EcsEntity* e);


        std::vector<EcsEntity*> entities;
    };
} // namespace brl


#endif // ENTITY_HPP
