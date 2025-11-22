# Remove only the glfw submodule
rm -rf ext/glfw .git/modules/ext/glfw

# Remove only glfw from .gitmodules
git config --file .gitmodules --remove-section submodule.ext/glfw 2>/dev/null || true

# Remove only glfw from .git/config
git config --remove-section submodule.ext/glfw 2>/dev/null || true
