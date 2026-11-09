# Info about integrations with Courage

## Download MolmoSpaces assets

```bash
# Install resources manager
uv pip install git+https://codeberg.org/robotics-folks/molmospaces-resources.git
# Download the assets
molmospaces-download --type mjcf --mode batch --install-dir assets-molmospaces/mjcf \
    --batch.objects thor \
    --batch.robots franka_droid g1 i2rt_yam \
    --batch.scenes ithor procthor-10k-val
```
