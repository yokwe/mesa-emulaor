package mesa.courier.program;

public class TypeArray extends Type {
	public final long size;
	public final Type type;
	
	public TypeArray(long size, Type type) {
		super(Kind.ARRAY);
		this.size = size;
		this.type = type;
	}
	
	public String toString() {
		return String.format("[%s (%d)%s]]", kind, size, type.toString());
	}
}
