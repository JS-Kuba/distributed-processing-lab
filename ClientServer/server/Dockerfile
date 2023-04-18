# Choose a base image with GCC installed
FROM gcc:latest
 
# Set the working directory
WORKDIR /app
 
# Copy the source code to the working directory
COPY server.c /app/server.c
 
# Compile the server code
RUN gcc -o server server.c -lpthread
 
# Expose the server port to the host
EXPOSE 12345
 
# Run the server when the container starts
CMD ["./server"]