"""This module in used by `invoke` to generate CLI tasks."""

from invoke import Collection, task
from ifxtasks import pypi

@task(
    help={
        "publish": "Publish the release in Artifactory.",
        "url": "The repository (package index) URL to upload the package to.",
    },
)
def release(
    ctx, publish=False, url=None
):
    """Release the package."""
    pypi.build_dist(ctx)
    if publish:
        pypi.push(ctx, _, url)

namespace = Collection(
    release
)
namespace.configure({"run": {"echo": True}})

