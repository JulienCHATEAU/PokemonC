# Vérification du nombre de paramètres
# Pour calculer le nombre de paramètres, on utilise la fonction ...
nbarg=ARGV.size
if (nbarg == 0) then abort "You need to specify a C file"; end

i = 0

while i < nbarg
  crt_file_name = ARGV[i]
  begin
    read_file =File.open("src/"+crt_file_name,"r")
  	rescue Errno::ENOENT
  		abort "\t[Error: Failed to open #{crt_file_name}]\n"
  end

  crt_file_header = crt_file_name.sub(".c", ".h")
  ouput_file_path = "src/" + crt_file_header

  begin
  	output_file =File.open(ouput_file_path,"w+")
  	rescue Errno::ENOENT
  		abort "\t[Error: Failed to open #{ouput_file_path}]\n"
  end

  header_define = crt_file_header.sub('.', '_').upcase()

  output_file.print("#ifndef #{header_define}\n#define #{header_define}\n\n")

  begin
    while line = read_file.readline
        if line =~ /(int|char|double|float|Boolean|bool|BagItem|void)\s.*(\(([^,\s]* [^,\s]*, )*.* .*\)|\(\)) {/
          output_file.print(line.sub(" {", ";"))
        end
    end
    rescue EOFError
  end

  output_file.print("\n#endif")

  i = i + 1
end

