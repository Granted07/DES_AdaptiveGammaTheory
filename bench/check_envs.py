"""check that PyTorch can see and use the NVIDIA GPU."""

import sys

import torch


def main() -> int:
    print(f"python            : {sys.version.split()[0]}")
    print(f"torch             : {torch.__version__}")
    print(f"torch built w/CUDA: {torch.version.cuda}")  # None => CPU-only build

    if not torch.cuda.is_available():
        print("cuda available    : False")
        if torch.version.cuda is None:
            print("-> this is a CPU-only PyTorch build; reinstall the CUDA build.")
        else:
            print("-> CUDA build installed but no usable GPU found; check the")
            print("   NVIDIA driver and that the laptop is not using only the iGPU.")
        return 1

    print("cuda available    : True")
    print(f"cudnn version     : {torch.backends.cudnn.version()}")

    n = torch.cuda.device_count()
    print(f"cuda devices      : {n}")
    for i in range(n):
        props = torch.cuda.get_device_properties(i)
        total_mb = props.total_memory / 1024**2
        print(f"  [{i}] {props.name}")
        print(f"      total VRAM        : {total_mb:.0f} MB")
        print(f"      compute capability: {props.major}.{props.minor}")
        print(f"      multiprocessors   : {props.multi_processor_count}")

    x = torch.randn(1024, 1024, device="cuda")
    y = x @ x
    torch.cuda.synchronize()
    print(f"test matmul ok    : {tuple(y.shape)} on {y.device}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
