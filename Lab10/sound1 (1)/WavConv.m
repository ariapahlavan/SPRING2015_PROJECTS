% Author: Ramesh Yerraballi
% Use this in octave to convert a wav file into a C declaration of the
% the samples with a chosen precision at 11.025kHz
% Run in octave as:
% WavConv ('filename', precision); 
% The inputs are (i) the name of the wav file without the extention .wav.
%                (ii) the sample precision in bits (e.g., 4 if using a 4-bit DAC)
% This will generate a filename.txt file that has the C declaration.
% For example, say you have a sound file called boom.wav that you want to use in
% in your game. 
%    1. Put the WavConv.m script and the boom.wav file in a folder (say sounds).
%    2. Open octave/matlab and cd to the sounds folder
%    3. Run script as: WavConv("boom", 4)
%       For 4-bit precision
%       This will produce a file named boom.txt, which contains a C declaration
%       for the array of samples. You can cut-and-paste it in your code
function WavConv(filename,precision)
  % 1. Read the file and extract the sample as well as the frequency:
  [Spls, fs] = audioread(strcat(filename,'.wav'));
  % 2. Downsample it, to get it to the frequency you want (11.025 kHz):
  Spls = downsample(Spls, round(fs/11025));
  % 3. Adjust the samples (originals are fractions between -1 and 1) to range between 0 and 15 (4-bit)
  %    If you want to change it to 6-bit uncomment the 6-bit 
  Spls = round((Spls+ 1)* (2^precision-1)/2); 
  % 4. write it to a file so you can cut and paste it into your C code:
  file = fopen(strcat(filename,'.txt'), 'w');
  fprintf(file, strcat('unsigned char ', filename,'[',num2str(length(Spls)),'] = {'));
  fprintf(file, '%d,', Spls(1:length(Spls)-1));
  fprintf(file, '%d', Spls(length(Spls)));
  fprintf(file, '};\n');
  fclose(file);
end


