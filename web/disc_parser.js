export class DreamcastDiscParser {
    constructor() {
        this.tracks = [];
    }

    // Process a .GDI text file descriptor to extract binary track addresses
    async parseGDIFile(gdiFile, siblingFiles) {
        const text = await gdiFile.text();
        const lines = text.split('\n');
        
        // The first line indicates the number of tracks (typically 3 or more)
        const trackCount = parseInt(lines[0].trim(), 10);
        this.tracks = [];

        for (let i = 1; i <= trackCount; i++) {
            if (!lines[i]) continue;
            
            // Format: track_num starting_sector mode sector_size filename [offset]
            const tokens = lines[i].trim().split(/\s+/);
            if (tokens.length < 5) continue;

            const trackNum = parseInt(tokens[0], 10);
            const sectorSize = parseInt(tokens[3], 10);
            const filename = tokens[4].replace(/"/g, ''); // Strip quotes

            // Find matching binary binary asset file inside matching input array
            const binaryFile = siblingFiles.find(f => f.name.toLowerCase() === filename.toLowerCase());
            
            if (binaryFile) {
                this.tracks.push({
                    number: trackNum,
                    sectorSize: sectorSize,
                    fileHandle: binaryFile,
                    buffer: await binaryFile.arrayBuffer()
                });
            }
        }
        console.log(`Parsed ${this.tracks.length} operational track sectors successfully.`);
        return this.tracks;
    }

    // Read a specific 2048 or 2352-byte hardware sector from an active track
    getSector(trackNum, sectorIndex) {
        const track = this.tracks.find(t => t.number === trackNum);
        if (!track) return null;

        const byteOffset = sectorIndex * track.sectorSize;
        if (byteOffset + track.sectorSize > track.buffer.byteLength) return null;

        return new Uint8Array(track.buffer, byteOffset, track.sectorSize);
    }
}
