# Remove ONLY the glfw entry from the index
git rm --cached ext/glfw

# If that fails with "is a directory" error, try:
git rm --cached -r ext/glfw

# Now add it as a submodule
git submodule add https://github.com/glfw/glfw.git ext/glfw