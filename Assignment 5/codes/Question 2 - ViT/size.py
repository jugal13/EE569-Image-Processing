import argparse
import datetime
import os
# from solver import Solver
from model import VisionTransformer
import torch


def main(args):
    os.makedirs(args.model_path, exist_ok=True)

    # solver.train()
    # solver.test()

    # Load the saved model
    model_path = './model/mnist/ViT_model.pt'
    device = torch.device('cuda' if torch.cuda.is_available() else 'cpu')
    state_dict = torch.load(model_path, map_location=device)

    model = VisionTransformer(n_channels=args.n_channels, embed_dim=args.embed_dim,
                              n_layers=args.n_layers, n_attention_heads=args.n_attention_heads,
                              forward_mul=args.forward_mul, image_size=args.image_size,
                              patch_size=args.patch_size, n_classes=args.n_classes)
    model.load_state_dict(state_dict)
    # print(model)
    # Count the parameters
    total_params = sum(p.numel()
                       for p in model.parameters() if p.requires_grad)

    print(f'Total parameters in the model: {total_params}')


def print_args(args):
    for k in dict(sorted(vars(args).items())).items():
        print(k)
    print()


def update_args(args):
    args.model_path = os.path.join(args.model_path, args.dataset)
    args.n_patches = (args.image_size // args.patch_size) ** 2
    args.is_cuda = torch.cuda.is_available()
    return args


if __name__ == '__main__':
    parser = argparse.ArgumentParser(
        description='Vision Transformer for MNIST')

    # Training Arguments
    parser.add_argument('--epochs', type=int, default=200)
    parser.add_argument('--warmup_epochs', type=int, default=10)
    parser.add_argument('--batch_size', type=int, default=256)
    parser.add_argument('--n_classes', type=int, default=10)
    parser.add_argument('--num_workers', type=int, default=4)
    parser.add_argument('--lr', type=float, default=5e-4)

    # Data arguments
    parser.add_argument('--dataset', type=str, default='mnist',
                        help=['mnist', 'fmnist', 'svhn', 'cifar10'])
    parser.add_argument("--image_size", type=int,
                        default=28, help="Image size")
    parser.add_argument("--patch_size", type=int, default=4, help="Patch Size")
    parser.add_argument("--n_channels", type=int,
                        default=1, help="Number of channels")
    parser.add_argument('--data_path', type=str, default='./data/')

    # ViT Arguments
    parser.add_argument("--embed_dim", type=int, default=64,
                        help="dimensionality of the latent space")
    parser.add_argument("--n_attention_heads", type=int,
                        default=4, help="number of heads to be used")
    parser.add_argument("--forward_mul", type=int,
                        default=2, help="forward multiplier")
    parser.add_argument("--n_layers", type=int, default=6,
                        help="number of encoder layers")
    parser.add_argument('--model_path', type=str, default='./model')
    parser.add_argument("--load_model", type=bool,
                        default=False, help="Load saved model")

    start_time = datetime.datetime.now()
    print("Started at " + str(start_time.strftime('%Y-%m-%d %H:%M:%S')))

    args = parser.parse_args()
    args = update_args(args)
    print_args(args)

    main(args)

    end_time = datetime.datetime.now()
    duration = end_time - start_time
    print("Ended at " + str(end_time.strftime('%Y-%m-%d %H:%M:%S')))
    print("Duration: " + str(duration))
