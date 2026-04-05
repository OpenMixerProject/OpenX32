import torch
import torch.nn as nn
import torch.optim as optim
import torchaudio
import numpy as np
import os

# Step 1: model-definition
class DeFeedbackNet(nn.Module):
    def __init__(self, input_size=16, hidden_size=32):
        super(DeFeedbackNet, self).__init__()
        self.fc1 = nn.Linear(input_size, hidden_size)
        self.relu = nn.ReLU()
        self.fc2 = nn.Linear(hidden_size, 1)

    def forward(self, x):
        x = self.fc1(x)
        x = self.relu(x)
        x = self.fc2(x)
        return x

# Step 2: prepare multi-file-processing
def prepare_multi_data(file_pairs, block_size=16):
    all_x = []
    all_y = []
    
    for dry_path, disturbed_path in file_pairs:
        print(f"Lade: {dry_path} / {disturbed_path}")
        clean, _ = torchaudio.load(dry_path)
        disturbed, _ = torchaudio.load(disturbed_path)
        
        min_len = min(clean.shape[1], disturbed.shape[1])
        clean = clean[0, :min_len]
        disturbed = disturbed[0, :min_len]
        
        num_blocks = min_len // block_size
        
        # slice into blocks
        x_clean = clean[:num_blocks*block_size].view(num_blocks, block_size)
        x_dist = disturbed[:num_blocks*block_size].view(num_blocks, block_size)
        
        all_x.append(x_clean)
        all_x.append(x_dist)
        
        # Labels: 0 for clean, 1 for feedbacked
        all_y.append(torch.zeros(num_blocks, 1))
        all_y.append(torch.ones(num_blocks, 1))
    
    # merge everything into one large tensor
    x = torch.cat(all_x, dim=0)
    y = torch.cat(all_y, dim=0)
    
    # shuffle
    indices = torch.randperm(x.size(0))
    return x[indices], y[indices]

# Step 3: Training-Loop with Multi-File Support
def train_model_multi(file_pairs):
    model = DeFeedbackNet()
    criterion = nn.BCEWithLogitsLoss()
    optimizer = optim.Adam(model.parameters(), lr=0.001) # set LR quite low for multiple input-data
    
    inputs, labels = prepare_multi_data(file_pairs)
    
    batch_size = 64 # use batches for more stable training
    num_batches = len(inputs) // batch_size
    
    print(f"Start training with {len(inputs)} blocks...")
    for epoch in range(50): # more data needs less epochs
        epoch_loss = 0
        for i in range(num_batches):
            start = i * batch_size
            end = start + batch_size
            
            batch_x = inputs[start:end]
            batch_y = labels[start:end]
            
            optimizer.zero_grad()
            outputs = model(batch_x)
            loss = criterion(outputs, batch_y)
            loss.backward()
            optimizer.step()
            epoch_loss += loss.item()
            
        if (epoch + 1) % 5 == 0:
            print(f"Epoch {epoch+1}, Avg Loss: {epoch_loss/num_batches:.4f}")
    
    return model

# Step 4: Export to header-file for SHARC DSP
def export_to_headerfile(model):
    w1 = model.fc1.weight.data.numpy()
    b1 = model.fc1.bias.data.numpy()
    w2 = model.fc2.weight.data.numpy().flatten()
    b2 = model.fc2.bias.data.numpy()[0]

    with open("coeffs.h", "w") as text_file:
        text_file.write("#ifndef COEFFS_H\n#define COEFFS_H\n\n")
        text_file.write(f"const float nn_weights_ih[32][16] = {{")
        for row in w1:
            text_file.write("{" + ", ".join([f"{v:.6f}f" for v in row]) + "},")
        text_file.write("};\n\n")
        
        text_file.write(f"const float nn_bias_h[32] = {{")
        text_file.write(", ".join([f"{v:.6f}f" for v in b1]) + "};\n\n")
    
        text_file.write(f"const float nn_weights_ho[32] = {{")
        text_file.write(", ".join([f"{v:.6f}f" for v in w2]) + "};\n\n")
        
        text_file.write(f"const float nn_bias_out = {b2:.6f}f;\n\n")
        text_file.write("#endif")

# list of training-audio-files (Dry, Disturbed)
my_files = [
    ('audio_ref.wav', 'audio_500Hz.wav'),
    ('audio_ref.wav', 'audio_750Hz.wav'),
    ('audio_ref.wav', 'audio_1000Hz.wav'),
    ('audio_ref.wav', 'audio_2000Hz.wav')
]

model = train_model_multi(my_files)
export_to_headerfile(model)