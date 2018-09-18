#include "stdafx.h"
#include "CMxStream.h"


ssize_t CMxStream::vlc_stream_Read(void *buf, size_t len) {
    
}
ssize_t CMxStream::vlc_stream_ReadPartial(void *buf, size_t len) {
    
}
ssize_t CMxStream::vlc_stream_Peek(const uint8_t **, size_t) {
    
}
CMxBlock* CMxStream::vlc_stream_ReadBlock() {
    
}
uint64_t CMxStream::vlc_stream_Tell() {
    
}
bool CMxStream::vlc_stream_Eof() {
    
}
int CMxStream::vlc_stream_Seek(uint64_t offset) {
    
}
int CMxStream::vlc_stream_vaControl(int query, va_list args) {
    
}
CMxBlock* CMxStream::vlc_stream_Block(size_t) {
    
}
char* CMxStream::vlc_stream_ReadLine() {
    
}
int CMxStream::vlc_stream_ReadDir(input_item_node_t *) {
    
}
void CMxStream::vlc_stream_Delete() {
    
}
CMxStream* CMxStream::vlc_stream_CommonNew(vlc_object_t *, void (*)(CMxStream *)) {
    
}
CMxStream* CMxStream::vlc_stream_MemoryNew(vlc_object_t *obj, uint8_t *base,size_t size, bool preserve) {
    
}
CMxStream* CMxStream::vlc_stream_NewURL(vlc_object_t *obj, const char *url) {
    
}
CMxStream*v CMxStream::lc_stream_fifo_New(vlc_object_t *parent) {
    
}
int CMxStream::vlc_stream_fifo_Queue(CMxStream *s, block_t *block) {
    
}
ssize_t CMxStream::vlc_stream_fifo_Write(const void *buf,size_t len) {
    
}
void CMxStream::vlc_stream_fifo_Close() {
    
}
CMxStream* CMxStream::vlc_stream_FilterNew( CMxStream *p_source, const char *psz_stream_filter ) {
    
}
int CMxStream::vlc_stream_FilterDefaultReadDir(input_item_node_t *p_node) {
    
}
