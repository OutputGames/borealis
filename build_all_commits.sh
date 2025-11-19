#!/usr/bin/env bash
set -e

rm -rf builds
mkdir -p builds

# List all commits (oldest first)
commits=$(git rev-list --reverse HEAD)

for commit in $commits; do
    echo "=== Building commit $commit ==="

    # Checkout commit quietly
    git checkout -q --force "$commit"

    # Clean up previous build folders
    rm -rf build out
    mkdir build

    # Detect Premake
    if [[ -f "premake5.lua" ]]; then
        echo "[INFO] premake5 found. Generating Visual Studio project..."
        ./premake5 vs2026 2>/dev/null || ./premake5.exe vs2026 2>/dev/null || true

        sln_file=$(ls *.sln 2>/dev/null | head -n 1)
        if [[ -n "$sln_file" ]]; then
            echo "[INFO] Building with MSBuild: $sln_file"
            "/c/Program Files/Microsoft Visual Studio/2022/Community/MSBuild/Current/Bin/MSBuild.exe" "$sln_file" //p:Configuration=Release //m //p:WarningLevel=0 //clp:ErrorsOnly || {
                echo "[WARN] Build failed for $commit (MSBuild)"
                read -p "Retry build? (y/n): " retry
                if [[ "$retry" =~ ^[Yy]$ ]]; then
                    echo "[INFO] Retrying build..."
                    rm -rf out
                    ./premake5 vs2026 2>/dev/null || ./premake5.exe vs2026 2>/dev/null || true
                    "/c/Program Files/Microsoft Visual Studio/2022/Community/MSBuild/Current/Bin/MSBuild.exe" "$sln_file" //p:Configuration=Release //m //p:WarningLevel=0 //clp:ErrorsOnly || {
                        echo "[ERROR] Retry failed. Skipping $commit."
                        rm -rf out
                        continue
                    }
                else
                    echo "[INFO] Skipping $commit."
                    rm -rf out
                    continue
                fi
            }
        else
            echo "[WARN] No .sln found after Premake. Skipping."
            rm -rf out
            continue
        fi
    else
        echo "[INFO] No premake found. Falling back to make..."
        make || {
            echo "[WARN] Build failed for $commit (make)"
            read -p "Retry build? (y/n): " retry
            if [[ "$retry" =~ ^[Yy]$ ]]; then
                echo "[INFO] Retrying build..."
                rm -rf out
                make || {
                    echo "[ERROR] Retry failed. Skipping $commit."
                    rm -rf out
                    continue
                }
            else
                echo "[INFO] Skipping $commit."
                rm -rf out
                continue
            fi
        }
    fi

    # Ensure out/ exists and contains an artifact
    # Ensure there's an artifact output directory or file
    if [[ ! -d "out" && ! -f "myapp.exe" ]]; then
        echo "[WARN] No out/ directory or base artifact found. Skipping."
        continue
    fi


    # Determine which executable exists
    artifact=""
    if [[ -f "out/borealis-test.exe" ]]; then
        artifact="out/borealis-test.exe"
    elif [[ -f "out/myapp.exe" ]]; then
        artifact="out/myapp.exe"
    elif [[ -f "myapp.exe" ]]; then
        artifact="myapp.exe"
    fi

    if [[ -z "$artifact" ]]; then
        echo "[WARN] No executable found in out/. Skipping copy."
        rm -rf out
        continue
    fi

    # Check for assets.res (also supports resources/ folder)
    # Check for resources
    resources=""
    if [[ -f "out/assets.res" ]]; then
        resources="out/assets.res"
    elif [[ -d "test/resources/" ]]; then
        resources="test/resources/"
    fi


    # Copy artifacts
    # Format folder name as: MM-DD (commit message) (hash)
    commit_date=$(git log -1 --date=local --date=format:"%m-%d_%H-%M" --format="%ad" "$commit")
    commit_msg=$(git log -1 --pretty=format:"%s" "$commit" | tr -cd '[:alnum:] ._-')
    short_hash=$(git rev-parse --short "$commit")

    output_dir="builds/${commit_date} (${commit_msg}) (${short_hash})"
    mkdir -p "$output_dir"

    mkdir -p "$output_dir"
    echo "[INFO] Copying $artifact → $output_dir"
    cp "$artifact" "$output_dir/" || true

    if [[ -n "$resources" ]]; then
        echo "[INFO] Copying resources..."
        if [[ -d "$resources" ]]; then
            mkdir -p "$output_dir/test/"
            cp -r "$resources" "$output_dir/test" || true
        else
            cp "$resources" "$output_dir/" || true
        fi
    fi

    # Save commit info
    git log -1 --pretty=format:"%h %s (%ci)" "$commit" > "$output_dir/commit_info.txt"

    # Clean up out/ after copying
    rm -rf out

    echo "[OK] Finished commit $commit"
done

# Restore main branch
git checkout -f -q main

echo "=== All builds complete! ==="
